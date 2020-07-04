#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>

struct info {
    const char *name;
    size_t total_drained;
};

void read_callback(struct bufferevent *bev, void *ctx)
{
    struct evbuffer* input = bufferevent_get_input(bev);
    size_t data_len = evbuffer_get_length(input);
    if (data_len)
    {
        char in_msg[1024];

        int result = evbuffer_remove(input, in_msg, data_len);
        if(result == -1) //failure
        {
            printf("received msg: %s\n", in_msg);
            return;
        }

        in_msg[data_len] = '\n';
        printf("length of msg %d bytes \n", (int)data_len);
        printf("received msg: %s\n", in_msg);
    }
}

void event_callback(struct bufferevent *bev, short events, void *ctx)
{
    struct evbuffer *input = bufferevent_get_input(bev);
    int finished = 0;

    if (events & BEV_EVENT_EOF) {
        size_t len = evbuffer_get_length(input);
        printf("socket closed.\n");
        finished = 1;
    }
    if (events & BEV_EVENT_ERROR) {
        printf("socket error: %s\n",
               evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
        finished = 1;
    }
    if (finished) {
        //free(ctx);
        bufferevent_free(bev);
    }
}

void on_conn_accepted(struct evconnlistener *listener,
                    evutil_socket_t fd,
                    struct sockaddr *address,
                    int socklen, void *arg)
{
    char addr[INET_ADDRSTRLEN];
    auto *sin = reinterpret_cast<sockaddr_in *>(address);
    inet_ntop(AF_INET, &sin->sin_addr, addr, INET_ADDRSTRLEN);
    std::cout << "Accept TCP connection from: " << addr << std::endl;

    /* We got a new connection! Set up a bufferevent for it. */
    struct event_base* base = evconnlistener_get_base(listener);
    struct bufferevent* buf_evt = bufferevent_socket_new(
            base, fd, BEV_OPT_CLOSE_ON_FREE);
    //This function returns a bufferevent on success, and NULL on failure.

    bufferevent_setcb(buf_evt, read_callback, NULL, event_callback, NULL);

    bufferevent_enable(buf_evt, EV_READ|EV_WRITE);
    //By default, a newly created bufferevent has writing enabled, but not reading.
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