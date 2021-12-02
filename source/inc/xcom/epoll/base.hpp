#pragma once
#ifndef PCH
    #include <cerrno>
    #include <iostream>
    #include <type_traits>
    #include <xcom/epoll/util.hpp>
#endif

namespace xcom::epoll
{
    /* Handler sample
    class Handler
    {
    public:
        /// @brief Returns requested actions, receiving and/or sending.
        io_flags_t io_flags() const noexcept;

        /// @brief Called after a session was successfully created.
        /// @returns True if the handler want to continue the execution. False otherwise.
        bool on_session_created(fd_t fd, const endpoint_t& remote_endpoint) noexcept;

        /// @brief Called after a session was closed.
        void on_session_closed(fd_t fd) noexcept;

        /// @brief Called if an error occurs.
        /// @returns True if IO flags were updated. False otherwise.
        bool on_error(fd_t fd, int error) noexcept;

        /// @brief It reads data available.
        /// @returns True if IO flags were updated. False otherwise.
        bool receive_data(fd_t fd) noexcept;

        /// @brief It sends data.
        /// @returns True if IO flags were updated. False otherwise.
        bool send_data(fd_t fd) noexcept;
    };
    */

    using start_error_t = std::uint8_t;

    template <typename Handler, std::uint32_t Max_client_count = 1u>
    class base
    {
    public:
        using handler_t = Handler;

        template <typename... Args>
        base(Args&&... args): _handler(std::forward<Args>(args)...)
        {
        }

        virtual ~base() noexcept(std::is_nothrow_destructible<handler_t>::value) { stop(); }

        const handler_t& handler() const noexcept { return _handler; }
        handler_t& handler() noexcept { return _handler; }

        virtual start_error_t start(endpoint_t endpoint) noexcept = 0;
        void stop() noexcept;

        error_t step_execution(int timeout_ms) noexcept;

    protected:
        virtual void process(int event_count) noexcept = 0;

        void process_event(fd_t fd, event_flags_t event_flags) noexcept;
        void close_session(fd_t fd) noexcept;

        handler_t _handler;
        fd_t _fd {0};
        fd_t _epoll_fd {0};
        std::array<event_t, Max_client_count * 2> _events {};
    };

    template <typename Handler, std::uint32_t Max_client_count>
    void base<Handler, Max_client_count>::stop() noexcept
    {
        if (_fd > 0)
        {
            close_session(_fd);
            auto result = util::close(_epoll_fd);
            if (result == an_error)
            {
                std::cout << "error " << errno << " closing epoll for session " << _fd << '\n';
            }
        }
    }

    template <typename Handler, std::uint32_t Max_client_count>
    void base<Handler, Max_client_count>::close_session(fd_t fd) noexcept
    {
        if (_epoll_fd > 0)
        {
            auto result = util::unregister_event(_epoll_fd, fd);
            if (result == an_error)
            {
                std::cout << "error " << errno << " unregistering event of session " << fd << '\n';
            }
        }

        if (fd > 0)
        {
            auto result = util::close(fd);
            if (result == an_error)
            {
                std::cout << "error " << errno << " closing session " << fd << '\n';
            }

            _handler.on_session_closed(fd);
        }
    }

    template <typename Handler, std::uint32_t Max_client_count>
    error_t base<Handler, Max_client_count>::step_execution(int timeout_ms) noexcept
    {
        // std::cout << "wait for events...\n";
        auto result = util::wait_for_events(_epoll_fd, _events.data(), static_cast<int>(_events.size()), timeout_ms);
        if (result > 0)
        {
            process(result);
        }
        else if (result < 0)
        {
            std::cout << "error " << errno << " waiting for events\n";
        }

        return result;
    }

    template <typename Handler, std::uint32_t Max_client_count>
    void base<Handler, Max_client_count>::process_event(fd_t fd, event_flags_t event_flags) noexcept
    {
        std::cout << "process events " << event_flags << " for session " << fd << '\n';
        bool flags_updated = util::receiving_flag(event_flags) ? _handler.receive_data(fd) : false;
        flags_updated |= util::sending_flag(event_flags) ? _handler.send_data(fd) : false;
        if (flags_updated)
        {
            auto required_event_flags = util::flags_for(_handler.io_flags());
            if ((required_event_flags ^ event_flags) != 0u)
            {
                if (required_event_flags != 0u)
                {
                    std::cout << "updated events to " << required_event_flags << " for session " << fd << '\n';
                    util::update_event(_epoll_fd, fd, required_event_flags);
                }
                else
                {
                    close_session(fd);
                }
            }
        }
    }
}
