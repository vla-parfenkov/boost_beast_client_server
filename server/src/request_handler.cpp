//
// Created by vladislav on 17.03.19.
//

#include "request_handler.h"
#include <regex>
#include <request_handler.h>


RequestHandler::RequestHandler(const std::string &root) : root(root)
{
    serverData = std::make_shared<ServerData> ();
    const std::string server_filename = "server.json";
    serverData->readDataFromFile(root + "/" + server_filename);
}

template<class Body, class Allocator, bool isRequest, class Fields>
void RequestHandler::handleRequest(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req,
                                   std::function<void(http::message<isRequest, Body, Fields> &&msg)> callBack) {

    if( req.method() != http::verb::get)
    {
        callBack(std::move(badRequest("Unknown HTTP-method", req)));
    }

    if( req.target().empty() ||
        req.target()[0] != '/' ||
        req.target().find("..") != beast::string_view::npos)
        callBack(std::move(badRequest("Illegal request-target", req)));


    auto body = createBody(req.target(), req.method());


    auto const size = body.size();

    http::response<http::file_body> res{
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(http::status::ok, req.version())};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    res.body() = body;
    callBack(std::move(res));
}

template<class Body, class Allocator>
http::response<http::string_body> RequestHandler::badRequest(beast::string_view why,
                                                             http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req) {
    http::response<http::string_body> res{http::status::bad_request, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.keep_alive(req.keep_alive());
    res.body() = "{ \"status\": 400, \"message\": \"" + std::string(why) + "\" }";
    res.prepare_payload();
    return res;
}

template<class Body, class Allocator>
http::response<http::string_body> RequestHandler::notFound(beast::string_view why,
                                                           http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req) {
    http::response<http::string_body> res {http::status::not_found, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.keep_alive(req.keep_alive());
    res.body() = "{ \"status\": 404, \"message\": \"" + std::string(why) + " was not found.\" }";
    res.prepare_payload();
    return res;
}

template<class Body, class Allocator>
http::response<http::string_body> RequestHandler::serverError(beast::string_view why,
                                                              http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req) {
    http::response<http::string_body> res{http::status::internal_server_error, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.keep_alive(req.keep_alive());
    res.body() = "{ \"status\": 500, \"message\": \"" + std::string(why) + "\" }";
    res.prepare_payload();
    return res;
}


RequestHandler::~RequestHandler() {

}

const std::string RequestHandler::createBody(const std::string &cmd, http::verb method) {
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

std::string RequestHandler::getAllEmployers(const std::string &cmd,
        std::shared_ptr<ServerData> serverData)
        {
    return "{ \"status\": 200, \"data\": [" + serverData->getAllEmployersAsString() + "] }";
}

std::string RequestHandler::getEmployerByName(const std::string &cmd,
        std::shared_ptr<ServerData> serverData)
{
    std::size_t found = cmd.find_last_of('/');
    return "{ \"status\": 200, \"data\": "
            + serverData->getEmployeeByNameAsJsonString(cmd.substr(found + 1)) + "}";
}
