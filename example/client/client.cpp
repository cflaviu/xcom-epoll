#ifndef PCH
    #include "client.hpp"
    #include <cstring>
    #include <sys/socket.h>
#endif

namespace xcom::epoll::example::client
{
    bool data_handler::on_session_created(fd_t fd, const endpoint_t& remote_endpoint) noexcept
    {
        std::cout << "session " << fd << " <" << remote_endpoint << "> created\n";
        return true;
    }

    void data_handler::on_session_closed(fd_t fd) noexcept { std::cout << "session " << fd << " closed\n"; }

    bool data_handler::receive_data(fd_t fd) noexcept
    {
        std::uint32_t buffer_size;
        auto result = util::receive(fd, reinterpret_cast<std::uint8_t*>(&buffer_size), 4);
        if (result != an_error)
        {
            result = recv(fd, _receiving_buffer.data(), buffer_size, 0);
            if (result != an_error)
            {
                std::cout << "received " << buffer_size << " bytes in session " << fd << '\n';
            }
            else
            {
                std::cout << "error-B " << errno << " receiving data in session " << fd << '\n';
            }
        }
        else
        {
            std::cout << "error-A " << errno << " receiving data in session " << fd << '\n';
        }

        _io_flags.sending = ++_sending_count <= _max_sending_count;
        return true;
    }

    bool data_handler::send_data(fd_t fd) noexcept
    {
        bool retry = false;
        bool can_continue = _sending_count <= _max_sending_count;
        if (can_continue)
        {
            std::cout << "sending #" << int(_sending_count) << '\n';
            std::uint32_t amount = _sending_buffer.size() / _sending_count;
            std::memcpy(_sending_buffer.data(), reinterpret_cast<const std::uint8_t*>(&amount), 4u);
            std::fill_n(_sending_buffer.begin() + 4, amount - 4, _sending_count);
            auto result = util::send(fd, _sending_buffer.data(), amount);
            if (result != an_error)
            {
                _io_flags.sending = false;
                std::cout << "sent " << amount << " bytes in session " << fd << '\n';
            }
            else if (util::try_again_or_would_block())
            {
                std::cout << "retrying retrieving data in session " << fd << '\n';
                retry = true;
            }
            else
            {
                can_continue = false;
                std::cout << "error " << errno << " sending data in session " << fd << '\n';
            }
        }

        _io_flags.receiving = can_continue && !retry;
        return true;
    }
}
