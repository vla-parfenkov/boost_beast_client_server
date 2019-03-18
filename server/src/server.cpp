//
// Created by vladislav on 11.03.19.
//

#include "server.h"

#include <mutex>
#include <iostream>

#include "session.h"

void Server::start()
{
    while (!isStop)
    {
        auto tcpSocket = std::make_unique<boost::asio::ip::tcp::socket>(ioService);
        boost::system::error_code error;
        tcpAcceptor.accept(*tcpSocket, error);

        if (error)
        {
            std::cerr << "Accept error" << std::endl;
            continue;
        }

        auto session = std::make_shared<Session>(std::move(tcpSocket), handler,
                                                                [this](std::shared_ptr<Session> session) {
                                                                    std::unique_lock<std::mutex> lock(sessionsMutex);
                                                                    sessions.erase(session);
                                                                });
        {
            std::unique_lock<std::mutex> lock(sessionsMutex);
            sessions.insert(session);
        }

        pool.addTask(std::bind(&Session::read, &*session));
    }
}

void Server::stop()
{
    isStop = true;
    std::unique_lock<std::mutex> lock(sessionsMutex);
    tcpAcceptor.close();
    for (auto& session: sessions)
        session->close();
}


Server::~Server() {
    stop();
}

Server::Server(const std::string &address, const std::string &port,
        const std::string &directory, size_t poolSize) : tcpAcceptor(ioService), pool(poolSize), handler(directory)
{

    boost::asio::ip::tcp::resolver resolver(ioService);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({address, port});
    tcpAcceptor.open(endpoint.protocol());
    tcpAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    tcpAcceptor.bind(endpoint);

    tcpAcceptor.listen();
}
