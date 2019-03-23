//
// Created by vladislav on 11.03.19.
//

#include "session.h"
#include <iostream>
#include "request_handler.h"


Session::Session (boost::asio::ip::tcp::socket&& socket, const std::string& root,
    std::function<void (std::shared_ptr<Session>)> abortedCallback) : stream(std::move(socket)),
        abortedCallback(std::move(abortedCallback)), lambda(*this), root(root) {

}

void Session::read()
{
    req = {};

    stream.expires_after(std::chrono::seconds(30));

    http::async_read(stream, buffer, req,
                     beast::bind_front_handler(
                             &Session::onRead,
                              shared_from_this()));
}


void Session::close()
{
    beast::error_code ec;
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    abortedCallback( shared_from_this() );
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
        abortedCallback( shared_from_this());
        return;
    }

    RequestHandler::handleRequest(root, std::move(req), lambda);
}


void Session::onWrite(bool close, beast::error_code error, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if(error) {
        std::cerr << error.message() << std::endl;
        abortedCallback(  shared_from_this() );
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



