#ifndef PCH
    #include "client.hpp"
#endif

int main(int /*argc*/, const char* /*argv*/[])
{
    using namespace xcom;
    using namespace xcom::epoll;

    example::client::item client(io_flags_t {true, false}, 4u * 1024u);
    constexpr endpoint_t remote_endpoint {ipv4_address_t {127, 0, 0, 1}, 9000u};

    auto result = client.start(remote_endpoint);
    if (result == 0u)
    {
        std::cout << "client started\n";
        for (auto count = 20u; count != 0u; --count)
        {
            client.step_execution(5000);
        }
    }
    else
    {
        std::cout << "client failed to start, error " << result << '\n';
    }

    std::cout << "client finished\n";
    return 0;
}
