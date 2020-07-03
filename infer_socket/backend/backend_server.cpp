#include <iostream>

#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

using namespace boost::placeholders;
using namespace boost::asio;

io_service boost_io_service;


class network_session : public boost::enable_shared_from_this<network_session>
        , boost::noncopyable {
    typedef network_session self_type;

    network_session(io_service& an_io_service) : sock_(an_io_service)
    {
    }

public:
    typedef boost::system::error_code error_code;
    typedef boost::shared_ptr<network_session> ptr;

    static ptr new_instance(io_service& an_io_service) {
        ptr new_inst(new network_session(an_io_service));
        return new_inst;
    }

    ip::tcp::socket& sock() { return sock_;}

    void start()
    {
        /*
        started_ = true;
        clients.push_back( shared_from_this());
        last_ping = boost::posix_time::microsec_clock::local_time();
        // first, we wait for client to login
        */
        do_read();
    }

private:
    void do_read()
    {
        /*
        async_read(sock_, buffer(read_buffer_),
                   MEM_FN2(read_complete,_1,_2), MEM_FN2(on_read,_1,_2));
        // post_check_ping();
        */
    }

    size_t read_complete(const boost::system::error_code & err, size_t bytes)
    {
        /*
        if ( err) return 0;
        bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
        // we read one-by-one until we get to enter, no buffering
        return found ? 0 : 1;
        */
        return 0;  // later to delete this line
    }

private:
    ip::tcp::socket sock_;
    /*
    enum { max_msg = 1024 };
    char read_buffer_[max_msg];
    char write_buffer_[max_msg];
    bool started_;
    deadline_timer timer_;
    boost::posix_time::ptime last_ping;
    bool clients_changed_;
    */
};

void handle_accept(ip::tcp::acceptor* acceptor,
        network_session::ptr session, const boost::system::error_code& err)
{
    session->start();
    network_session::ptr new_session =
            network_session::new_instance(boost_io_service);
    acceptor->async_accept(new_session->sock(),
            boost::bind(handle_accept, acceptor, new_session, _1));
}

int main()
{
    int server_port = 8001;

    ip::tcp::acceptor acceptor(boost_io_service,
            ip::tcp::endpoint(ip::tcp::v4(), server_port));

    network_session::ptr session_ptr = network_session::new_instance(boost_io_service);
    acceptor.async_accept(session_ptr->sock(),
            boost::bind(handle_accept, &acceptor, session_ptr, _1));
    boost_io_service.run();

    return 0;
}
