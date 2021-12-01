#include "server.hpp"

int main(int /*argc*/, const char* /*argv*/[])
{
    using namespace xcom;
    using namespace xcom::epoll::example::server;

    data_handler handler({false, true}, 4u * 1024u);
    item server(std::move(handler));

    endpoint_t listener_endpoint {};
    listener_endpoint.address = xcom::ipv4_address_t {127, 0, 0, 1};
    listener_endpoint.port = 9000u;

    {
        auto result = server.start(listener_endpoint);
        if (result == 0u)
        {
            std::cout << "sever started\n";
            for (auto count = 25u; count != 0u; --count)
            {
                server.step_execution(5000);
            }
        }
        else
        {
            std::cout << "server failed to start, error " << result << '\n';
        }
    }

    std::cout << "server finished\n";
    return 0;
}
