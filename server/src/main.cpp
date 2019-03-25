#include <iostream>
#include <regex>
#include <boost/program_options.hpp>
#include "server.h"

int main(int argc, char* argv[]) 
{

    std::string rootDir;
    std::string port;
    std::string addr;
    size_t poolSize;

    boost::program_options::options_description descGeneric("Generic Options");
    boost::program_options::options_description descServer("Server Configuration");

    boost::program_options::options_description cmdlineOptions;

    // Generic
    descGeneric.add_options()
        ("help,h", "Produce help message")
    ;
    
    // Server
    descServer.add_options()
        ("pool_size, t", boost::program_options::value<size_t>(&poolSize)->default_value(4), "ThreadPool size")
        ("address, a", boost::program_options::value<std::string>(&addr)->default_value("localhost"), "Address")
        ("port, p", boost::program_options::value<std::string>(&port)->default_value("5000"), "Port")
        ("rootDir, r", boost::program_options::value<std::string>(&rootDir)->default_value(getenv("HOME")), "Root dir")
    ;

    cmdlineOptions.add(descGeneric).add(descServer);

    boost::program_options::variables_map vm;
    boost::program_options::store(
            boost::program_options::command_line_parser(argc, argv).options(cmdlineOptions).allow_unregistered().run(),
            vm);
    boost::program_options::notify(vm);

    auto server = std::make_unique<Server>(addr, port, rootDir, poolSize);

    std::cout << "Server start" << std::endl;
    try {
        server->start();
    } catch (const std::exception& exc)
    {
        std::cerr << exc.what() << std::endl;
    }


    return 0;
}