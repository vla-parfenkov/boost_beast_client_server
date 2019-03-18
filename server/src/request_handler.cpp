//
// Created by vladislav on 17.03.19.
//

#include <request_handler.h>

#include "request_handler.h"

RequestHandler::RequestHandler(const std::string &root) : root(root) {

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
        return send(bad_request("Illegal request-target"));

    std::string path = path_cat(root, req.target());
    if(req.target().back() == '/')
        path.append("index.html");



    // Cache the size since we need it after the move
    auto const size = body.size();

    // Respond to GET request
    http::response<http::file_body> res{
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(http::status::ok, req.version())};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    callBack(std::move(res));
}

template<class Body, class Allocator>
http::response<http::string_body> RequestHandler::badRequest(beast::string_view why,
                                                             http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req) {
    http::response<http::string_body> res{http::status::bad_request, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
}

template<class Body, class Allocator>
http::response<http::string_body> RequestHandler::notFound(beast::string_view why,
                                                           http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req) {
    http::response<http::string_body> res {http::status::not_found, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    //res.body() = "The resource '" + std::string(target) + "' was not found.";
    res.prepare_payload();
    return res;
}

template<class Body, class Allocator>
http::response<http::string_body> RequestHandler::serverError(beast::string_view why,
                                                              http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req) {
    http::response<http::string_body> res{http::status::internal_server_error, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "An error occurred: '" + std::string(why) + "'";
    res.prepare_payload();
    return res;
}

template<class Body, class Allocator>
http::response<http::string_body> RequestHandler::createBody(const std::string &cmd,
                                                             http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req) {
    return boost::beast::http::response<boost::beast::http::basic_string_body<char, std::char_traits<char>, std::allocator<char>>, boost::beast::http::basic_fields<std::allocator<char>>>();
}

RequestHandler::~RequestHandler() {

}
