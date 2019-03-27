//
// Created by vladislav on 26.03.19.
//


#include "responce_parser.h"
#include "boost/property_tree/json_parser.hpp"

ProtocolParser::ProtocolParser() {

}

ProtocolParser::~ProtocolParser() {

}

const std::string ProtocolParser::createJsonRequest(GeneralCmd generalcmd, SubCmd subcmd, std::vector<std::string> argv) {
    boost::property_tree::ptree req;
    std::string  oper;
    std::stringstream stream;

    switch (generalcmd)
    {
        case GET: {
            oper.append(generalcmd_to_string.at(GET));
            break;
        }
        case HELP:
        case STOP:
        case UNKNOWN_GENERAL_CMD:
        default:
            throw std::invalid_argument("Unsupported operaion");
    }
    oper.append("-");
    switch (subcmd)
    {
        case NAME: {
            oper.append(subcmd_to_string.at(NAME));
            req.put("oper", oper);
            req.put("name", argv.front());
            break;
        }
        case ALL:
        {
            oper.append(subcmd_to_string.at(ALL));
            req.put("oper", oper);
            break;
        }
        case UNKNOWN_SUB_CMD:
        default:
            throw std::invalid_argument("Unsupported operaion");
    }

    boost::property_tree::write_json(stream, req);

    return stream.str();

}
