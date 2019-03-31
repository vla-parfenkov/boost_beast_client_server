//
// Created by vladislav on 26.03.19.
//

#ifndef SAMPLE_CLIENT_CLIENT_H
#define SAMPLE_CLIENT_CLIENT_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>



namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class Client 
{
private:
    net::io_context ioc;
    tcp::resolver resolver;
    beast::tcp_stream stream;

    std::string host;

    const size_t version = 11;

public:
    Client();
    ~Client();

    const std::string send(const std::string& msg);
    void open(const std::string& host, const std::string& port);
    void close();

};


#endif //SAMPLE_CLIENT_CLIENT_H
