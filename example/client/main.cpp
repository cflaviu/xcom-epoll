#include "client.hpp"

int main(int /*argc*/, const char* /*argv*/[])
{
    using namespace xcom;
    using namespace xcom::epoll::example::client;

    data_handler handler;
    item client(std::move(handler));

    endpoint_t remote_endpoint {};
    remote_endpoint.address = ipv4_address_t {0, 0, 0, 0};
    remote_endpoint.port = 2021u;

    if (client.start(remote_endpoint) == 0u)
    {
        for (auto count = 100u; count != 0u; --count)
        {
            client.step_execution(5000);
        }
    }

    return 0;
}
