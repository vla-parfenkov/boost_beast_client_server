//
// Created by vladislav on 17.03.19.
//

#ifndef SAMPLE_SERVER_REQUEST_HANDLER_H
#define SAMPLE_SERVER_REQUEST_HANDLER_H


#include <string>
#include <boost/beast.hpp>
#include <vector>
#include <unordered_map>

#include "server_data.h"

namespace beast = boost::beast;
namespace http = beast::http;

class RequestHandler {
private:

    template<class Body, class Allocator>
    static http::response<http::string_body> badRequest(beast::string_view why,
            http::request<Body, http::basic_fields<Allocator>>&& req);

    template<class Body, class Allocator>
    static http::response<http::string_body> notFound(beast::string_view why,
            http::request<Body, http::basic_fields<Allocator>>&& req);

    template<class Body, class Allocator>
    static http::response<http::string_body> serverError(beast::string_view why,
            http::request<Body, http::basic_fields<Allocator>>&& req);

    static const std::string createBody(const std::string& root, const std::string& cmd,
            http::verb method);

    static std::string getAllEmployers(const std::string& cmd, std::shared_ptr<ServerData> serverData);
    static std::string getEmployerByName(const std::string& cmd, std::shared_ptr<ServerData> serverData);

    mutable std::unordered_map<std::string, std::string> ext_to_mime =
            {
                    { "html", "text/html" },
                    { ".json", "application/json" }
            };

public:
    RequestHandler();
    ~RequestHandler();

    template<class Body, class Allocator, class Send>
    static void handleRequest(const std::string& root, http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send);
};


#endif //SAMPLE_SERVER_REQUEST_HANDLER_H
