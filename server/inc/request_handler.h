//
// Created by vladislav on 17.03.19.
//

#ifndef SAMPLE_SERVER_REQUEST_HANDLER_H
#define SAMPLE_SERVER_REQUEST_HANDLER_H


#include <string>
#include <boost/beast.hpp>
#include <vector>
#include <unordered_map>
#include <regex>


#include "server_data.h"


namespace beast = boost::beast;
namespace http = beast::http;

class RequestHandler 
{
private:
    std::string root;
    std::shared_ptr<ServerData> serverData;

    template<class Send>
    void badRequest(beast::string_view why, Send&& send, size_t version, bool keep_alive);

    template<class Send>
    void notFound(beast::string_view what, Send&& send, size_t version, bool keep_alive);

    template<class Send>
    void serverError(beast::string_view why, Send&& send, size_t version, bool keep_alive);

    const std::string createBody (beast::string_view&& view,
            http::verb&& method,const  std::string& body);

    static const std::string getAllEmployers(const std::string& cmd, std::shared_ptr<ServerData> serverData);
    static const std::string getEmployerByName(const std::string& cmd, std::shared_ptr<ServerData> serverData);

    mutable std::unordered_map<std::string, std::string> ext_to_mime =
            {
                    { "html", "text/html" },
                    { ".json", "application/json" }
            };

    mutable std::unordered_map<std::string,
            const std::string(*)(const std::string&, std::shared_ptr<ServerData> ) > get_action =
            {
                    { "/\s*all", getAllEmployers },
                    { "/\s*name/(.*?([a-zA-Z]+|\\d+)$)", getEmployerByName }
            };

public:
    RequestHandler(const std::string& root);
    ~RequestHandler();

    template<class Body, class Allocator, class Send>
    void handleRequest(http::request<Body, http::basic_fields<Allocator>>&& req,Send&& send);
};


#endif //SAMPLE_SERVER_REQUEST_HANDLER_H
