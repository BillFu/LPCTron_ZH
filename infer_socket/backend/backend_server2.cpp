#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/thread.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>

#define BOOST_THREAD_VERSION 4

#include <boost/thread.hpp>
#include <boost/chrono.hpp>

struct info {
    const char *name;
    size_t total_drained;
};

void wait(int seconds)
{
    boost::this_thread::sleep_for(boost::chrono::seconds{seconds});
}

void do_inference(struct bufferevent *buf_event)
{
    printf("tts inference is in running.\n");

    for (int i = 0; i < 5; ++i)
    {
        wait(1);
        std::cout << i << '\n';
    }

    printf("tts inference is done.\n");

    char reply_msg[] = "tts done";
    size_t size_reply = strlen(reply_msg);

    int result2 = bufferevent_write(buf_event, reply_msg, size_reply);
    if(result2 == -1) //failure
    {
        printf("error occurred when to send out reply.\n");
        return;
    }
    else
    {
        printf("reply has been sent out.\n");
    }

    int result3 = bufferevent_flush(buf_event, EV_WRITE, BEV_FLUSH);
    printf("the result to call flush is: %d\n", result3);

}

// Remember that the thread must be joined or detached before its
// destructor is called. Otherwise, your program will terminate!

// BOOST_THREAD_VERSION=2

void read_callback(struct bufferevent *buf_event, void *ctx)
{
    struct evbuffer* input = bufferevent_get_input(buf_event);
    size_t data_len = evbuffer_get_length(input);
    if (data_len)
    {
        char in_msg[1024];

        int result = evbuffer_remove(input, in_msg, data_len);
        if(result == -1) //failure
        {
            printf("error occurred to extract the incoming message.\n");
            return;
        }

        in_msg[data_len] = '\n';
        printf("length of msg %d bytes \n", (int)data_len);
        printf("received msg: %s\n", in_msg);

        boost::thread(boost::bind(
                do_inference, buf_event)).detach();
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

    int iOptval = 1;
    int result = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *) &iOptval, sizeof(int));

    /* We got a new connection! Set up a bufferevent for it. */
    struct event_base* base = evconnlistener_get_base(listener);
    struct bufferevent* buf_evt = bufferevent_socket_new(
            base, fd, BEV_OPT_CLOSE_ON_FREE);
    //This function returns a bufferevent on success, and NULL on failure.

    bufferevent_setcb(buf_evt, read_callback, NULL, event_callback, NULL);
    bufferevent_enable(buf_evt, BEV_OPT_THREADSAFE );
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

    evthread_use_pthreads();

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