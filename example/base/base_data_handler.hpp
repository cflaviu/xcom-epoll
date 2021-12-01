#pragma once
#ifndef PCH
    #include <iostream>
    #include <vector>
    #include <xcom/epoll/util.hpp>
#endif

namespace xcom::epoll::example
{
    class data_handler
    {
    public:
        data_handler(io_flags_t io_flags, size_t buffer_size):
            _sending_buffer(buffer_size),
            _receiving_buffer(buffer_size),
            _io_flags(io_flags)
        {
        }

        virtual ~data_handler() noexcept = default;

        io_flags_t io_flags() const noexcept { return _io_flags; }

        bool on_error(fd_t fd, int error) noexcept
        {
            std::cout << "error " << error << " in session " << fd << '\n';
            return true;
        }

    protected:
        using byte_array_t = std::vector<std::uint8_t>;

        byte_array_t _sending_buffer;
        byte_array_t _receiving_buffer;
        io_flags_t _io_flags {};
    };
}
