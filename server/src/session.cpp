//
// Created by vladislav on 11.03.19.
//

#include "session.h"
#include "request_handler.h"
#include <iostream>


Session::Session (boost::asio::ip::tcp::socket&& socket, const RequestHandler& requestHandler,
    std::function<void (std::shared_ptr<Session>)> abortedCallback) : stream(std::move(socket)),
        abortedCallback(std::move(abortedCallback)), lambda(*this), requestHandler(requestHandler) {

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

    requestHandler.handleRequest(std::move(req), lambda);
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



RequestHandler::RequestHandler(const std::string& root) : root(root)
{
    serverData = std::make_shared<ServerData> ();
    const std::string server_filename = "server.json";
    serverData->readDataFromFile(root + "/" + server_filename);
}

template<class Body, class Allocator, class Send>
void RequestHandler::handleRequest(http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req, 
                                    Send&& send) {

    if( req.method() != http::verb::get)
    {
        badRequest("Unknown HTTP-method", send, req.version(), req.keep_alive());
        return;
    }

    if( req.target().empty() ||
        req.target()[0] != '/' ||
        req.target().find("..") != beast::string_view::npos)
    {
        badRequest("Illegal request-target", send, req.version(), req.keep_alive());
        return;
    }


    auto body = createBody(req.target(), req.method());


    auto const size = body.size();

    http::response<http::string_body> res{
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(http::status::ok, req.version())};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    res.body() = body;
    send(std::move(res));
}

template <class Send>
void RequestHandler::badRequest(beast::string_view why, Send&& send, size_t version, 
        bool keep_alive) 
{
    http::response<http::string_body> res{http::status::bad_request, version};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.keep_alive(keep_alive);
    res.body() = "{ \"status\": 400, \"message\": \"" + std::string(why) + "\" }";
    res.prepare_payload();
    send(std::move(res));
}

template <class Send>
void RequestHandler::notFound(beast::string_view what, Send&& send, size_t version, 
        bool keep_alive) 
{
    http::response<http::string_body> res {http::status::not_found, version};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.keep_alive(keep_alive);
    res.body() = "{ \"status\": 404, \"message\": \"" + std::string(what) + " was not found.\" }";
    res.prepare_payload();
    send(std::move(res));
}

template <class Send>
void RequestHandler::serverError(beast::string_view why, Send&& send, size_t version, 
        bool keep_alive) 
{
    http::response<http::string_body> res{http::status::internal_server_error, version};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.keep_alive(keep_alive);
    res.body() = "{ \"status\": 500, \"message\": \"" + std::string(why) + "\" }";
    res.prepare_payload();
    send(std::move(res));
}


RequestHandler::~RequestHandler() {

}

const std::string RequestHandler::createBody(beast::string_view&& view, http::verb&& method)
 {
    std::string cmd(view);
    //routing
    switch (method)
    {
        case http::verb::get:
        {
            for (auto &act : get_action)
            {
                std::regex regex(act.first);
                if (std::regex_match(cmd, regex))
                {
                    return act.second(cmd, serverData);
                }
            }
        }
        default:
            return  "{ \"status\": 500, \"message\": \"Method don't support\" }";
    }
}

const std::string RequestHandler::getAllEmployers(const std::string &cmd,
        std::shared_ptr<ServerData> serverData)
        {
    return "{ \"status\": 200, \"data\": [" + serverData->getAllEmployersAsString() + "] }";
}

const std::string RequestHandler::getEmployerByName(const std::string &cmd,
        std::shared_ptr<ServerData> serverData)
{
    std::size_t found = cmd.find_last_of('/');
    return "{ \"status\": 200, \"data\": "
            + serverData->getEmployeeByNameAsJsonString(cmd.substr(found + 1)) + "}";
}
