//
// Created by vladislav on 11.03.19.
//

#include "session.h"
#include <iostream>
#include <boost/beast/core/bind_handler.hpp>



Session::Session(std::unique_ptr<boost::asio::ip::tcp::socket> socket,
        const RequestHandler& requestHandler,
        std::function<void (std::shared_ptr<Session>)> abortedCallback) : stream(std::move(socket)),
        abortedCallback(std::move(abortedCallback)),
        handler(requestHandler) {

}

void Session::read()
{
    req = {};

    stream.expires_after(std::chrono::seconds(30));

    http::async_read(stream, buffer, req,
                     beast::bind_handler(
                             &Session::onRead,
                             std::shared_ptr<Session>(this)));
}


void Session::close()
{
    beast::error_code ec;
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    abortedCallback( std::shared_ptr<Session>(this));
}

void Session::onRead(beast::error_code error, std::size_t bytesTransferred)
{
    boost::ignore_unused(bytesTransferred);

    if(error == http::error::end_of_stream)
    {
        close();
        return;
    }

    if(error)
    {
        std::cerr << error.message()  << std::endl;
        abortedCallback(std::shared_ptr<Session>(this));
        return;
    }

    handler.handleRequest(std::move(req), std::bind(&Session::write, this, std::placeholders::_1));
}

template<bool isRequest, class Body, class Fields>
void Session::write(http::message<isRequest, Body, Fields> &&msg)
{
    auto sp = std::make_shared<http::message<isRequest, Body, Fields>>(std::move(msg));

    res = sp;

    http::async_write(stream, *sp,
            beast::bind_handler(
                    &Session::onWrite,
                    std::shared_ptr<Session>(this),
                    sp->need_eof()));
}

void Session::onWrite(bool close, beast::error_code error, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if(error) {
        std::cerr << error.message() << std::endl;
        abortedCallback( std::shared_ptr<Session>(this));
        return;
    }

    if(close)
    {
        this->close();
        return;
    }

    res = nullptr;

    read();
}



