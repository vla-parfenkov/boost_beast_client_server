#include <iostream>
#include <regex>
#include "server.h"

int main(int argc, char* argv[]) {
    std::string rootDir(getenv("HOME"));
    rootDir = rootDir.substr(0, rootDir.find_last_of('/'));
    std::string addr = "localhost";
    std::string port = "5000";
    size_t poolSize = 4;

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