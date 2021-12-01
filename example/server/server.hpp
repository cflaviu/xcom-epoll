#pragma once
#include "base_data_handler.hpp"
#include <xcom/epoll/server.hpp>

namespace xcom::epoll::example::server
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
        std::uint32_t _amount {0};
        fd_t _listening_fd {0};
    };

    using item = epoll::server<data_handler, 10u>;
}
