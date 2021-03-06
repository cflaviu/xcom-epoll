// xcom-epoll library
#pragma once
#ifndef PCH
    #include <xcom/epoll/base.hpp>
#endif

namespace xcom::epoll
{
    template <typename Handler>
    class client: public base<Handler>
    {
    public:
        using parent = base<Handler>;
        using parent::parent;

        start_error_t start(endpoint_t endpoint) noexcept override;

    protected:
        using parent::_epoll_fd;
        using parent::_events;
        using parent::_fd;
        using parent::_handler;

        void process(int event_count) noexcept override;
    };

    template <typename Handler>
    start_error_t client<Handler>::start(endpoint_t endpoint) noexcept
    {
        auto result = util::connect(_fd, _epoll_fd, util::flags_for(_handler.io_flags()), endpoint);
        if (result == util::connect_error_t::none)
        {
            _handler.on_session_created(_fd, endpoint);
        }

        return start_error_t(result);
    }

    template <typename Handler>
    void client<Handler>::process(int event_count) noexcept
    {
        std::cout << "processing " << event_count << " event(s)\n";
        for (auto i = _events.begin(); event_count != 0u; --event_count, ++i)
        {
            auto event_flags = i->events;
            auto fd = i->data.fd;
            if (util::no_error(event_flags))
            {
                parent::process_event(fd, event_flags);
            }
            else if (_handler.on_error(fd, errno))
            {
                parent::stop();
            }
        }
    }
}
