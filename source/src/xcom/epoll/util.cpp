#ifndef PCH
    #include "xcom/epoll/util.hpp"
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <iostream>
    //#include <linux/socket.h>
    #include <netinet/in.h>
    #include <sys/epoll.h>
    #include <unistd.h>
#endif

namespace xcom::epoll::util
{
    bool no_error(event_flags_t flags) noexcept { return (flags & EPOLLERR) == 0u; }

    bool receiving_flag(event_flags_t flags) noexcept { return (flags & EPOLLIN) != 0u; }

    bool sending_flag(event_flags_t flags) noexcept { return (flags & EPOLLOUT) != 0u; }

    event_flags_t flags_for(bool receiving, bool sending) noexcept
    {
        event_flags_t flags = 0;
        if (receiving)
            flags |= EPOLLIN;
        if (sending)
            flags |= EPOLLOUT;
        return flags;
    }

    event_flags_t flags_for(io_flags_t input) noexcept { return flags_for(input.receiving, input.sending); }

    int set_non_blocking(int fd) noexcept
    {
        int flags = ::fcntl(fd, F_GETFL);
        if (flags != an_error)
        {
            flags = ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        }

        return flags;
    }

    // Register EPOLLIN on file descriptor FD into the epoll kernel event table indicated by epoll_fd,
    // and the parameter enable_et specifies whether et mode is enabled for FD
    int add_fd(int epoll_fd, int fd, int io_flags) noexcept
    {
        event_t event;
        event.data.fd = fd;
        event.events = EPOLLET | io_flags;                 // Registering the fd is readable
        int result = register_event(epoll_fd, fd, &event); // Register the fd with the epoll kernel event table
        if (result != an_error)
        {
            result = set_non_blocking(fd);
            if (result == an_error)
            {
                close(fd);
                std::cout << "error " << errno << " in set_non_blocking\n";
            }
        }
        else
        {
            std::cout << "error " << errno << " adding epoll fd\n";
        }

        return result;
    }

    int register_event(int epoll_fd, int fd, event_t* event) noexcept { return ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, event); }

    int modify_event(int epoll_fd, int fd, event_t* event) noexcept { return ::epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, event); }

    int unregister_event(int epoll_fd, int fd) noexcept { return ::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr); }

    int update_event(int epoll_fd, int fd, event_flags_t flags) noexcept
    {
        epoll_event event {};
        event.data.fd = fd;
        event.events = flags;
        return modify_event(epoll_fd, fd, &event);
    }

    bool try_again_or_would_block() noexcept { return (errno == EAGAIN) || (errno == EWOULDBLOCK); }

    int accept_new_connection(int listener_fd, endpoint_t& endpoint) noexcept
    {
        sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);
        int new_socket = ::accept(listener_fd, (sockaddr*) &client_address, &client_address_length);
        if (new_socket != an_error)
        {
            auto result = set_non_blocking(new_socket);
            if (result != an_error)
            {
                ipv4_address_t addr;
                *reinterpret_cast<std::uint32_t*>(addr.data()) = client_address.sin_addr.s_addr;
                endpoint.address = addr;
                endpoint.port = htons(client_address.sin_port);
            }
            else
            {
                ::close(new_socket);
                new_socket = result;
            }
        }
        else if (try_again_or_would_block())
        {
            std::cout << "accept returned EAGAIN or EWOULDBLOCK\n";
        }

        return new_socket;
    }

    int wait_for_events(int epoll_fd, event_t* events, int event_count, int timeout) noexcept
    {
        return ::epoll_wait(epoll_fd, events, event_count, timeout);
    }

    bool convert(const endpoint_t& endpoint, sockaddr_in& address) noexcept
    {
        address.sin_family = AF_INET;
        if (endpoint.address.index() == 0) // IPv4
        {
            const auto& src_address = std::get<ipv4_address_t>(endpoint.address);
            address.sin_addr.s_addr = *reinterpret_cast<const std::uint32_t*>(src_address.data());
        }
        else
        {
            return false;
        }

        address.sin_port = htons(endpoint.port);
        return true;
    }

    connect_error_t connect(int& fd, int& epoll_fd, int io_flags, endpoint_t& endpoint) noexcept
    {
        sockaddr_in address {};
        if (!convert(endpoint, address))
        {
            return connect_error_t::ipv6_not_supported;
        }

        fd = ::socket(PF_INET, SOCK_STREAM, 0);
        if (fd == an_error)
        {
            return connect_error_t::socket_create;
        }

        int result = connect(fd, (sockaddr*) &address, sizeof(address));
        if (result == an_error)
        {
            return connect_error_t::socket_connect;
        }

        endpoint.port = htons(address.sin_port);
        epoll_fd = ::epoll_create1(0);
        if (epoll_fd != an_error)
        {
            int result = add_fd(epoll_fd, fd, io_flags);
            if (result == an_error)
            {
                ::close(epoll_fd);
                ::close(fd);
                return connect_error_t::epoll_add_event;
            }
        }
        else
        {
            ::close(fd);
            return connect_error_t::epoll_create;
        }

        return connect_error_t::none;
    }

    listen_error_t listen(int& fd, int& epoll_fd, int event_count, int flags, const endpoint_t& endpoint) noexcept
    {
        sockaddr_in address {};
        if (!convert(endpoint, address))
        {
            return listen_error_t::ipv6_not_supported;
        }

        fd = ::socket(PF_INET, SOCK_STREAM, 0);
        if (fd == an_error)
        {
            return listen_error_t::socket_create;
        }

        auto bind_result = ::bind(fd, reinterpret_cast<const sockaddr*>(&address), sizeof(address));
        if (bind_result == an_error)
        {
            ::close(fd);
            return listen_error_t::socket_bind;
        }

        auto listen_result = ::listen(fd, event_count);
        if (listen_result == an_error)
        {
            ::close(fd);
            return listen_error_t::socket_listen;
        }

        epoll_fd = ::epoll_create1(0);
        if (epoll_fd == an_error)
        {
            ::close(fd);
            return listen_error_t::epoll_create;
        }

        int result = add_fd(epoll_fd, fd, flags); // Add listen file descriptor to event table using
        if (result == an_error)
        {
            ::close(epoll_fd);
            ::close(fd);
            return listen_error_t::epoll_add_event;
        }

        return listen_error_t::none;
    }

    int close(int fd) noexcept { return ::close(fd); }

    constexpr int data_transfer_flags = 0; // MSG_NOSIGNAL

    int receive(int fd, void* buffer, size_t buffer_size) { return recv(fd, buffer, buffer_size, data_transfer_flags); }

    int send(int fd, void* buffer, size_t buffer_size) { return ::send(fd, buffer, buffer_size, data_transfer_flags); }
}
