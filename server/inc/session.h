//
// Created by vladislav on 11.03.19.
//

#ifndef SAMPLE_SERVER_SESSION_H
#define SAMPLE_SERVER_SESSION_H

#include "request_handler.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <memory>


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>

class Session : std::enable_shared_from_this<Session>
{
private:
    struct SendLambda
    {
        Session& self;

        explicit
        SendLambda(Session& self)
            : self(self)
        {
        }

        template<bool isRequest, class Body, class Fields>
        void
        operator()(http::message<isRequest, Body, Fields>&& msg) const
        {
            auto sp = std::make_shared<
                http::message<isRequest, Body, Fields>>(std::move(msg));

            self.res = sp;

            http::async_write(
                self.stream,
                *sp,
                beast::bind_front_handler(
                    &Session::onWrite,
                    self.shared_from_this(),
                    sp->need_eof()));
        }
    };

    beast::tcp_stream stream;
    beast::flat_buffer buffer;
    http::request<http::string_body> req;
    std::shared_ptr<void> res;
    SendLambda lambda;
    std::string root;

    std::function<void (std::shared_ptr<Session>)> abortedCallback;

    void onRead(beast::error_code error, std::size_t bytesTransferred);
    void onWrite(bool close, beast::error_code error, std::size_t bytes_transferred);
public:
    Session (boost::asio::ip::tcp::socket&& socket, const std::string& root,
            std::function<void (std::shared_ptr<Session>)> abortedCallback);

    ~Session() = default;

    void read();
    void close();
};


#endif //SAMPLE_SERVER_SESSION_H
