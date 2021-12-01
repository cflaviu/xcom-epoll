#pragma once
#include "base_data_handler.hpp"
#include <xcom/epoll/client.hpp>

namespace xcom::epoll::example::client
{
    class data_handler: public example::data_handler
    {
    public:
        using parent = example::data_handler;
        using parent::parent;

        bool on_session_created(fd_t fd, const endpoint_t& remote_endpoint) noexcept;
        void on_session_closed(fd_t fd) noexcept;

        bool receive_data(fd_t fd) noexcept;
        bool send_data(fd_t fd) noexcept;

    protected:
        std::uint8_t _sending_count {1};
        std::uint8_t _max_sending_count {10};
    };

    using item = epoll::client<data_handler>;
}
