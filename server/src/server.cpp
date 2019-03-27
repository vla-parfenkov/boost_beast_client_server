//
// Created by vladislav on 11.03.19.
//

#include "server.h"

#include <mutex>
#include <iostream>
#include <boost/bind.hpp>

#include "session.h"

void Server::start()
{
    accept();

    try {
        for (size_t i = 0; i < poolSize; ++i)
        {
            threads.push_back(std::thread(boost::bind(&boost::asio::io_service::run, &ioService)));        
        }
    } catch (...)
    {
        throw std::runtime_error("threads create" + std::string(strerror(errno)));
    }

    for (auto &thread: threads)
           thread.join();
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
        const std::string &directory, size_t poolSize) : tcpAcceptor(ioService), poolSize(poolSize),
        requestHandler(directory)
{
    boost::asio::ip::tcp::resolver resolver(ioService);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({address, port});
    tcpAcceptor.open(endpoint.protocol());
    tcpAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    tcpAcceptor.bind(endpoint);

    tcpAcceptor.listen();

}


void Server::onAccept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket) 
{
    
    if (ec) 
    {
        std::cerr << "Accept error: " << ec << std::endl;
    } else 
    {
        //std::cout << "onAccept" << std::endl; 
        auto session = std::make_shared<Session>(std::move(socket), requestHandler,
                                                            [this](std::shared_ptr<Session> session) {
                                                                std::unique_lock<std::mutex> lock(sessionsMutex);
                                                                sessions.erase(session);
                                                            });

    {
            std::unique_lock<std::mutex> lock(sessionsMutex);
            sessions.insert(session);
    }
            session->read();
    }

    accept();
}


void Server::accept() 
{
    //std::cout << "accept" << std::endl; 

    tcpAcceptor.async_accept(
            boost::asio::make_strand(ioService),
            boost::beast::bind_front_handler(
                &Server::onAccept,
                this));
}