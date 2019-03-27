//
// Created by vladislav on 26.03.19.
//

#ifndef SAMPLE_CLIENT_RESPONCE_PARSER_H
#define SAMPLE_CLIENT_RESPONCE_PARSER_H

#include <boost/property_tree/ptree.hpp>
#include <unordered_map>
#include "cmd_parser.h"

class ProtocolParser {
private:
    mutable std::unordered_map<GeneralCmd, std::string> generalcmd_to_string =
            {
                    { GET, "get" },
            };

    mutable std::unordered_map<SubCmd, std::string> subcmd_to_string =
            {
                    { NAME, "name" },
                    { ALL, "all"},
            };

public:
    ProtocolParser();
    ~ProtocolParser();

    const std::string createJsonRequest(GeneralCmd generalcmd, SubCmd  subcmd, std::vector<std::string> argv);

};


#endif //SAMPLE_CLIENT_RESPONCE_PARSER_H
