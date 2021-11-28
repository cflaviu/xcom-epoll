#pragma once
#ifndef PCH
    #include <xcom/epoll/base.hpp>
#endif

namespace xcom::epoll
{
    template <typename Handler, std::uint32_t Max_client_count>
    class server: public base<Handler, Max_client_count>
    {
    public:
        using parent = base<Handler, Max_client_count>;
        using parent::parent;

        start_error_t start(const endpoint_t& endpoint) noexcept override;

    protected:
        using parent::_epoll_fd;
        using parent::_events;
        using parent::_fd;
        using parent::_handler;

        void process(int event_count) noexcept override;
        void create_session() noexcept;
    };

    template <typename Handler, std::uint32_t Max_client_count>
    start_error_t server<Handler, Max_client_count>::start(const endpoint_t& endpoint) noexcept
    {
        return start_error_t(util::listen(_fd, _epoll_fd, static_cast<int>(_events.size()), endpoint));
    }

    template <typename Handler, std::uint32_t Max_client_count>
    void server<Handler, Max_client_count>::process(int event_count) noexcept
    {
        std::cout << "processing " << event_count << " events\n";
        for (auto i = _events.begin(); event_count != 0u; --event_count, ++i)
        {
            auto event_flags = i->events;
            auto fd = i->data.fd;
            bool existing_connection = fd != _fd;
            if (util::no_error(event_flags))
            {
                if (existing_connection)
                {
                    parent::process_event(fd, event_flags);
                }
                else
                {
                    create_session();
                }
            }
            else if (existing_connection)
            {
                if (_handler.on_error(fd, errno))
                {
                    util::unregister_event(_epoll_fd, fd);
                }
            }
            else if (_handler.on_error(0, errno))
            {
                parent::stop();
            }
        }
    }

    template <typename Handler, std::uint32_t Max_client_count>
    void server<Handler, Max_client_count>::create_session() noexcept
    {
        endpoint_t remote_endpoint;
        auto new_fd = util::accept_new_connection(_fd, remote_endpoint.address);
        if (new_fd != an_error)
        {
            auto io_flags = _handler.get_initial_io_flags();
            auto result = util::add_fd(_epoll_fd, new_fd, util::flags_for(io_flags));
            if (result != an_error)
            {
                if (!_handler.on_session_created(new_fd, remote_endpoint)) {}
                {
                    util::unregister_event(_epoll_fd, new_fd);
                    util::close(new_fd);
                }
            }
            else
            {
                // report error
            }
        }
        else
        {
            // report error
        }
    }
}
