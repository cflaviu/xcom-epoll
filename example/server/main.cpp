#include "server.hpp"

int main(int /*argc*/, const char* /*argv*/[])
{
    using namespace xcom;
    using namespace xcom::epoll::example::server;

    data_handler handler;
    item server(std::move(handler));

    endpoint_t listener_endpoint {};
    listener_endpoint.address = xcom::ipv4_address_t {0, 0, 0, 0};
    listener_endpoint.port = 2021u;

    if (server.start(listener_endpoint) == 0u)
    {
        for (auto count = 100u; count != 0u; --count)
        {
            server.step_execution(5000);
        }
    }

    return 0;
}
