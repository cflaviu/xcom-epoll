#pragma once
#ifndef PCH
    #include <array>
    #include <cstdint>
    #include <variant>
#endif

namespace xcom
{
    using error_t = std::int32_t;
    using ipv4_address_t = std::array<std::uint8_t, 4>;
    using ipv6_address_t = std::array<std::uint8_t, 16>;
    using ip_address_t = std::variant<ipv4_address_t, ipv6_address_t>;
    using port_t = std::uint16_t;
    using fd_t = std::int32_t;

    constexpr fd_t an_error = -1;

    struct endpoint_t
    {
        ip_address_t address {};
        port_t port {0};
    };
}
