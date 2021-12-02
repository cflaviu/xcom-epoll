#ifndef PCH
    #include "xcom/type.hpp"
    #include <cstdint>
    #include <iostream>
    #include <variant>
#endif

namespace xcom
{
    std::ostream& operator<<(std::ostream& out, const ip_address_t& item)
    {
        if (item.index() == 0) // IPv4
        {
            const auto& ipv4_address = std::get<ipv4_address_t>(item);
            out << std::uint16_t(ipv4_address[0]) << '.' << std::uint16_t(ipv4_address[1]) << '.' << std::uint16_t(ipv4_address[2]) << '.'
                << std::uint16_t(ipv4_address[3]);
        }
        else
        {
            out << "<ipv6>";
        }

        return out;
    }

    std::ostream& operator<<(std::ostream& out, const endpoint_t& item)
    {
        out << item.address << ':' << item.port;
        return out;
    }
}
