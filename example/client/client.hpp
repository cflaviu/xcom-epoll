#pragma once
#include <xcom/epoll/client.hpp>

namespace xcom::epoll::example::client
{
    class data_handler
    {
    public:
        io_flags get_initial_io_flags() const noexcept;

        bool on_session_created(fd_t fd, const endpoint_t& remote_endpoint) noexcept;
        void on_session_closed(fd_t fd) noexcept;
        bool on_error(fd_t fd, int error) noexcept;

        bool receive_data(fd_t fd) noexcept;
        bool send_data(fd_t fd) noexcept;
    };

    using item = epoll::client<data_handler>;
}
