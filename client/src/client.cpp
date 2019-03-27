//
// Created by vladislav on 26.03.19.
//

#include <iostream>
#include "client.h"

Client::Client() : resolver(ioc), stream(ioc) {

}

Client::~Client() {

}

const std::string Client::send(const std::string &msg) {
    http::request<http::string_body> req{http::verb::post, "/", version};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::content_type, "application/json");
    req.body() = msg;
    req.prepare_payload();

    std::cout << "send -> " << msg << std::endl;

    try {

        http::write(stream, req);

        beast::flat_buffer buffer;

        http::response<http::string_body> res;

        http::read(stream, buffer, res);

        std::cout << "receive <- " << res.body() << std::endl;

        return std::string(res.body());
    } catch (const boost::beast::system_error& er)
    {
        throw er;
    }
}

void Client::open(const std::string &host, const std::string &port)
{
    auto const results = resolver.resolve(host, port);
    stream.connect(results);
    this->host = host;
}

void Client::close()
{
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    if(ec && ec != beast::errc::not_connected)
        std::cerr << ec << std::endl;
}
