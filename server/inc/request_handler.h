//
// Created by vladislav on 17.03.19.
//

#ifndef SAMPLE_SERVER_REQUEST_HANDLER_H
#define SAMPLE_SERVER_REQUEST_HANDLER_H


#include <string>
#include <boost/beast.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

class RequestHandler {
private:
    std::string root;

    template<class Body, class Allocator>
    http::response<http::string_body> badRequest(beast::string_view why,
            http::request<Body, http::basic_fields<Allocator>>&& req);

    template<class Body, class Allocator>
    http::response<http::string_body> notFound(beast::string_view why,
            http::request<Body, http::basic_fields<Allocator>>&& req);

    template<class Body, class Allocator>
    http::response<http::string_body> serverError(beast::string_view why,
            http::request<Body, http::basic_fields<Allocator>>&& req);

    template<class Body, class Allocator>
    http::response<http::string_body> createBody(const std::string& cmd,
            http::request<Body, http::basic_fields<Allocator>>&& req);

    mutable std::unordered_map<std::string, std::string> ext_to_mime =
            {
                    { "html", "text/html" },
                    { ".json", "application/json" }
            };

public:
    RequestHandler(const std::string& root);
    ~RequestHandler();

    template<class Body, class Allocator, bool isRequest, class Fields>
    void handleRequest(http::request<Body, http::basic_fields<Allocator>>&& req,
            std::function<void(http::message<isRequest, Body, Fields> &&msg)> callBack);
};


#endif //SAMPLE_SERVER_REQUEST_HANDLER_H
