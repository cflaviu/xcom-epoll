// xcom-epoll library
#pragma once
#ifndef PCH
    #include <sys/epoll.h>
    #include <xcom/type.hpp>
#endif

struct sockaddr_in;

namespace xcom::epoll
{
    using event_flags_t = std::uint32_t;
    using event_t = epoll_event;

    struct io_flags_t
    {
        bool sending {false};
        bool receiving {false};
    };
}

namespace xcom::epoll::util
{
    enum class listen_error_t : std::uint16_t
    {
        none,
        socket_create,
        socket_bind,
        socket_listen,
        epoll_create,
        epoll_add_event,
        ipv6_not_supported,
    };

    enum class connect_error_t : std::uint16_t
    {
        none,
        socket_create,
        socket_connect,
        epoll_create,
        epoll_add_event,
        ipv6_not_supported,
    };

    bool convert(const endpoint_t& endpoint, sockaddr_in& address) noexcept;
    event_flags_t flags_for(bool receiving, bool sending) noexcept;
    event_flags_t flags_for(io_flags_t input) noexcept;
    bool no_error(event_flags_t flags) noexcept;
    bool receiving_flag(event_flags_t flags) noexcept;
    bool sending_flag(event_flags_t flags) noexcept;
    int set_non_blocking(int fd) noexcept;
    int add_fd(int epoll_fd, int fd, int io_flags) noexcept;
    int register_event(int epoll_fd, int fd, event_t* event) noexcept;
    int modify_event(int epoll_fd, int fd, event_t* event) noexcept;
    int unregister_event(int epoll_fd, int fd) noexcept;
    int update_event(int epoll_fd, int fd, event_flags_t flags) noexcept;
    bool try_again_or_would_block() noexcept;
    int accept_new_connection(int listener_fd, endpoint_t& endpoint) noexcept;
    int wait_for_events(int _epoll_fd, event_t* events, int event_count, int timeout) noexcept;
    connect_error_t connect(int& fd, int& epoll_fd, int io_flags, endpoint_t& endpoint) noexcept;
    listen_error_t listen(int& listen_fd, int& epoll_fd, int max_connection_count, int flags, const endpoint_t& endpoint) noexcept;
    int close(int fd) noexcept;
    int receive(int fd, void* buffer, size_t buffer_size);
    int send(int fd, void* buffer, size_t buffer_size);
}
