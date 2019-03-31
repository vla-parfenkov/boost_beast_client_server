//
// Created by vladislav on 11.03.19.
//

#ifndef SAMPLE_SERVER_SERVER_H
#define SAMPLE_SERVER_SERVER_H

#include <boost/asio.hpp>
#include <set>
#include <thread>

#include "threadpool.h"
#include "session.h"
#include "request_handler.h"

class Server 
{
private:
    boost::asio::io_service ioService;
    boost::asio::ip::tcp::acceptor tcpAcceptor;
    std::mutex sessionsMutex;
    RequestHandler requestHandler;

    std::atomic_bool isStop;
    size_t poolSize;
    std::vector<std::thread> threads;
    std::set<std::shared_ptr<Session>> sessions;

    void onAccept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);
    void accept();
public:
    void start();
    void stop();

    Server(const std::string& address, const std::string & port,
         const std::string& directory, size_t poolSize);
    ~Server();
};


#endif //SAMPLE_SERVER_SERVER_H
