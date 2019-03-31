#include <iostream>
#include <boost/program_options.hpp>
#include "cmd_parser.h"
#include "responce_parser.h"
#include "client.h"


int main(int argc, char* argv[]) 
{

    std::string port;
    std::string addr;

    try 
    {

        boost::program_options::options_description desc("Options");
        desc.add_options()
                ("help,h", "Produce help message")
                ("address, a", boost::program_options::value<std::string>(&addr)->default_value("localhost"),
                        "Address Server")
                ("port, p", boost::program_options::value<std::string>(&port)->default_value("5000"), "Port")
                ;


        boost::program_options::options_description cmdline_options;
        cmdline_options.add(desc);
        boost::program_options::variables_map vm;
        store(boost::program_options::command_line_parser(argc, argv).options(cmdline_options).run(), vm);
        notify(vm);

        if (vm.count("help")) 
        {
            std::cout << desc << std::endl;
            return 0;
        }

        std::cout << "Client start" << std::endl;
    } catch (const std::exception &exc)
    {
        std::cerr << exc.what() << std::endl;
    }

    std::string cmd;
    auto cmdParser = std::make_unique<CmdParser>();
    auto requestPars = std::make_unique<ProtocolParser>();
    auto client = std::make_unique<Client>();
    while(std::getline(std::cin, cmd))
    {
        try 
        {
            std::string msg;
            std::vector<std::string> argv;
            cmdParser->parse(cmd);
            if (cmdParser->getGeneralCmd() == STOP)
            {
                break;
            }
            if (cmdParser->getGeneralCmd() == HELP 
                    || cmdParser->getGeneralCmd() >= UNKNOWN_GENERAL_CMD) 
            {
                std::cout << cmdParser->getDescGeneral() << std::endl;
                std::cout << cmdParser->getDescGet() << std::endl;
                continue;
            }

            if (cmdParser->getSubCmd() == NAME)
            {
                argv.push_back(cmdParser->getVm()["name"].as<std::string>());
            }
            try {
                msg = requestPars->createJsonRequest(cmdParser->getGeneralCmd(),
                     cmdParser->getSubCmd(), argv);
            } catch (const std::invalid_argument& er)
            {
                std::cout << cmdParser->getDescGeneral() << std::endl;
                std::cout << cmdParser->getDescGet() << std::endl;
                continue;
            }
            client->open(addr, port);
            client->send(msg);
            client->close();
        } catch (const std::exception& ex)
        {
           std::cerr << ex.what() << std::endl;
        }


    }
    return 0;
}