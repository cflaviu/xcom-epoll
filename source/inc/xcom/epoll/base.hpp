#pragma once
#ifndef PCH
    #include <cerrno>
    #include <iostream>
    #include <xcom/epoll/util.hpp>
#endif

namespace xcom::epoll
{
    /* Handler sample
    class Handler
    {
    public:
        ~Handler() noexcept;

        io_flags get_initial_io_flags() const noexcept;

        bool on_session_created(fd_t fd, const endpoint_t& remote_endpoint) noexcept;
        void on_session_closed(fd_t fd) noexcept;

        /// @returns True to close the connection; on_session_closed will not be called. False otherwise.
        bool on_error(fd_t fd, int error) noexcept;

        /// @returns True to continue receiving data. False otherwise.
        bool receive_data(fd_t fd) noexcept;

        /// @returns True to continue sending data. False otherwise.
        bool send_data(fd_t fd) noexcept;
    };
    */

    using start_error_t = std::uint8_t;

    template <typename Handler, std::uint32_t Max_client_count = 1u>
    class base
    {
    public:
        using handler_t = Handler;

        base(handler_t&& handler): _handler(std::move(handler)) {}
        virtual ~base() noexcept { stop(); }

        const handler_t& handler() const noexcept { return _handler; }
        handler_t& handler() noexcept { return _handler; }

        virtual start_error_t start(const endpoint_t& endpoint) noexcept = 0;
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
            util::close(_epoll_fd);
        }
    }

    template <typename Handler, std::uint32_t Max_client_count>
    void base<Handler, Max_client_count>::close_session(fd_t fd) noexcept
    {
        auto result = util::unregister_event(_epoll_fd, fd);
        util::close(fd);
        _handler.on_session_closed(fd);
        if (result == an_error)
        {
            // report error
        }
    }

    template <typename Handler, std::uint32_t Max_client_count>
    error_t base<Handler, Max_client_count>::step_execution(int timeout_ms) noexcept
    {
        std::cout << "wait for events...\n";
        auto result = util::wait_for_events(_epoll_fd, _events.data(), static_cast<int>(_events.size()), timeout_ms);
        if (result > 0)
        {
            process(result);
        }

        return result;
    }

    template <typename Handler, std::uint32_t Max_client_count>
    void base<Handler, Max_client_count>::process_event(fd_t fd, event_flags_t event_flags) noexcept
    {
        bool request_receiving = util::receiving_flag(event_flags) ? _handler.receive_data(fd) : false;
        bool request_sending = util::sending_flag(event_flags) ? _handler.send_data(fd) : false;
        auto required_event_flags = util::flags_for(request_receiving, request_sending);
        if ((required_event_flags ^ event_flags) != 0u)
        {
            if (required_event_flags != 0u)
            {
                util::update_event(_epoll_fd, fd, required_event_flags);
            }
            else
            {
                close_session(fd);
            }
        }
    }
}
