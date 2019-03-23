//
// Created by vladislav on 11.03.19.
//

#ifndef SAMPLE_SERVER_SESSION_H
#define SAMPLE_SERVER_SESSION_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include "request_handler.h"

namespace beast = boost::beast;
namespace http = boost::beast::http; 
namespace net = boost::asio;

class Session {
private:
    beast::tcp_stream stream;
    beast::flat_buffer buffer;
    http::request<http::string_body> req;
    std::shared_ptr<void> res;

    RequestHandler handler;

    std::function<void (std::shared_ptr<Session>)> abortedCallback;

    void onRead(beast::error_code error, std::size_t bytesTransferred);
    void onWrite(bool close, beast::error_code error, std::size_t bytes_transferred);
public:
    Session(std::unique_ptr<boost::asio::ip::tcp::socket> socket,
            const RequestHandler& requestHandler,
            std::function<void (std::shared_ptr<Session>)> abortedCallback);

    ~Session() = default;

    void read();
    template<bool isRequest, class Body, class Fields>
    void write(http::message<isRequest, Body, Fields>&& msg);
    void close();
};


#endif //SAMPLE_SERVER_SESSION_H
