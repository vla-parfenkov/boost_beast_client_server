//
// Created by vladislav on 17.03.19.
//

#ifndef SAMPLE_SERVER_REQUEST_HANDLER_H
#define SAMPLE_SERVER_REQUEST_HANDLER_H


#include <string>
#include <boost/beast.hpp>
#include <vector>

#include "server_data.h"

namespace beast = boost::beast;
namespace http = beast::http;

class RequestHandler {
private:
    std::string root;

    std::shared_ptr<ServerData> serverData;

    template<class Body, class Allocator>
    http::response<http::string_body> badRequest(beast::string_view why,
            http::request<Body, http::basic_fields<Allocator>>&& req);

    template<class Body, class Allocator>
    http::response<http::string_body> notFound(beast::string_view why,
            http::request<Body, http::basic_fields<Allocator>>&& req);

    template<class Body, class Allocator>
    http::response<http::string_body> serverError(beast::string_view why,
            http::request<Body, http::basic_fields<Allocator>>&& req);

    const std::string createBody(const std::string& cmd,
            http::verb method);

    static std::string getAllEmployers(const std::string& cmd, std::shared_ptr<ServerData> serverData);
    static std::string getEmployerByName(const std::string& cmd, std::shared_ptr<ServerData> serverData);

    mutable std::unordered_map<std::string, std::string> ext_to_mime =
            {
                    { "html", "text/html" },
                    { ".json", "application/json" }
            };

    mutable std::unordered_map<std::string,
            std::string(*)(const std::string&, std::shared_ptr<ServerData> ) > get_action =
            {
                    { "/\s*all", getAllEmployers },
                    { "/\s*name/(.*?([a-zA-Z]+|\\d+)$)", getEmployerByName }
            };

public:
    RequestHandler(const std::string& root);
    ~RequestHandler();

    template<class Body, class Allocator, bool isRequest, class Fields>
    void handleRequest(http::request<Body, http::basic_fields<Allocator>>&& req,
            std::function<void(http::message<isRequest, Body, Fields> &&msg)> callBack);
};


#endif //SAMPLE_SERVER_REQUEST_HANDLER_H
