#ifndef PCH
    #include "server.hpp"
    #include <cstring>
    #include <linux/socket.h>
    #include <sys/socket.h>
#endif

namespace xcom::epoll::example::server
{
    bool data_handler::on_session_created(fd_t fd, const endpoint_t& remote_endpoint) noexcept
    {
        std::cout << (fd != 3 ? "client" : "server") << " session " << fd << " <" << remote_endpoint << "> created\n";
        return true;
    }

    void data_handler::on_session_closed(fd_t fd) noexcept
    {
        std::cout << (fd != 3 ? "client" : "server") << " session " << fd << " closed\n";
    }

    bool data_handler::receive_data(fd_t fd) noexcept
    {
        std::cout << "receiving data in session " << fd << '\n';
        _amount = 0;
        bool retry = false;
        auto result = util::receive(fd, reinterpret_cast<std::uint8_t*>(&_amount), 4);
        if (result != an_error)
        {
            if (_amount != 0)
            {
                result = util::receive(fd, _receiving_buffer.data(), _amount);
                if (result != an_error)
                {
                    _io_flags.sending = true;
                    std::cout << "received " << _amount << " bytes in session " << fd << '\n';
                }
                else if (util::try_again_or_would_block())
                {
                    std::cout << "retrying retrieving data in session " << fd << '\n';
                    retry = true;
                }
                else
                {
                    std ::cout << "error-B " << errno << " receiving data in session " << fd << '\n';
                }
            }
        }
        else
        {
            std::cout << "error-A " << errno << " receiving data in session " << fd << '\n';
        }

        _io_flags.receiving = retry;
        return true;
    }

    bool data_handler::send_data(fd_t fd) noexcept
    {
        std::cout << "sending data in session " << fd << '\n';
        bool retry = false;
        bool can_continue = _amount != 0;
        if (can_continue)
        {
            std::memcpy(_sending_buffer.data(), reinterpret_cast<const std::uint8_t*>(&_amount), 4u);
            std::fill_n(_sending_buffer.begin() + 4, _amount, 0xFE);
            auto result = send(fd, _sending_buffer.data(), _amount + 4, 0);
            if (result != an_error)
            {
                _io_flags.sending = false;
                std::cout << "sent " << _amount << " bytes in session " << fd << '\n';
            }
            else if (util::try_again_or_would_block())
            {
                retry = true;
                std::cout << "retry sending data in session " << fd << '\n';
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
