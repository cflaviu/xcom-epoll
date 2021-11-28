#include "server.hpp"

namespace xcom::epoll::example::server
{
    io_flags data_handler::get_initial_io_flags() const noexcept { return {}; }

    bool data_handler::on_session_created(fd_t fd, const endpoint_t& remote_endpoint) noexcept { return true; }

    void data_handler::on_session_closed(fd_t fd) noexcept {}

    bool data_handler::on_error(fd_t fd, int error) noexcept { return true; }

    bool data_handler::receive_data(fd_t fd) noexcept { return false; }

    bool data_handler::send_data(fd_t fd) noexcept { return false; }
}
