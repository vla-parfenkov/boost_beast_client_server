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

    try {

        boost::program_options::options_description desc("Options");
        desc.add_options()
                ("help,h", "Produce help message")
                ("pool_size, t", boost::program_options::value<size_t>(&poolSize)->default_value(4), "ThreadPool size")
                ("address, a", boost::program_options::value<std::string>(&addr)->default_value("localhost"), "Address")
                ("port, p", boost::program_options::value<std::string>(&port)->default_value("5000"), "Port")
                ("rootDir, r", boost::program_options::value<std::string>(&rootDir)->default_value(getenv("HOME")),
                 "Root dir")
                 ;


        boost::program_options::options_description cmdline_options;
        cmdline_options.add(desc);
        boost::program_options::variables_map vm;
        store(boost::program_options::command_line_parser(argc, argv).options(cmdline_options).run(), vm);
        notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }

        auto server = std::make_unique<Server>(addr, port, rootDir, poolSize);

        std::cout << "Server start" << std::endl;
        server->start();
    } catch (const std::exception &exc)
    {
        std::cerr << exc.what() << std::endl;
    }


    return 0;
}