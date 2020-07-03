#include <event2/listener.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>


void on_conn_accepted(struct evconnlistener *listener,
                    evutil_socket_t fd,
                    struct sockaddr *address,
                    int socklen, void *arg)
{
    char addr[INET_ADDRSTRLEN];
    auto *sin = reinterpret_cast<sockaddr_in *>(address);
    inet_ntop(AF_INET, &sin->sin_addr, addr, INET_ADDRSTRLEN);
    std::cout << "Accept TCP connection from: " << addr << std::endl;
}

void accept_error_cb(struct evconnlistener *listener, void *arg)
{
    auto *base = evconnlistener_get_base(listener);
    // 跨平台的错误处理
    int err = EVUTIL_SOCKET_ERROR();
    std::cerr << "Got an error on the listener: "
              << evutil_socket_error_to_string(err)
              << std::endl;

    event_base_loopexit(base, NULL);
}

int main()
{
    short port = 8001;
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);

    auto* base = event_base_new();
    auto* listener = evconnlistener_new_bind(
            base, // event loop
            on_conn_accepted, // callback function, invoked when a new connect request accepted
            nullptr, // 传递给回调函数的参数
            LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1,
            reinterpret_cast<struct sockaddr *>(&sin), sizeof(sin)
    );

    if (listener == nullptr)
    {
        std::cerr << "Couldn't create socket listener" << std::endl;
        return 1;
    }

    evconnlistener_set_error_cb(listener, accept_error_cb);
    event_base_dispatch(base);

    return 0;
}