//
// Created by vladislav on 11.03.19.
//

#ifndef SAMPLE_SERVER_SERVER_H
#define SAMPLE_SERVER_SERVER_H

#include <boost/asio.hpp>
#include <set>

#include "threadpool.h"
#include "session.h"
#include "request_handler.h"

class Server {
private:
    boost::asio::io_service ioService;
    boost::asio::ip::tcp::acceptor tcpAcceptor;
    std::mutex sessionsMutex;
    RequestHandler requestHandler;

    std::atomic_bool isStop;
    ThreadPool pool;
    std::set<std::shared_ptr<Session>> sessions;

public:
    void start();
    void stop();

    Server(const std::string& address, const std::string & port, const std::string& directory, size_t poolSize);
    ~Server();
};


#endif //SAMPLE_SERVER_SERVER_H
