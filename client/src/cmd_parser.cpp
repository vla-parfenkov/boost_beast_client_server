//
// Created by vladislav on 26.03.19.
//


#include "cmd_parser.h"

#include <boost/tokenizer.hpp>
#include <boost/bind.hpp>

CmdParser::CmdParser() : generalCmd(UNKNOWN_GENERAL_CMD), subCmd(UNKNOWN_SUB_CMD)
{
    descGeneral.add_options()
            ("help,h", "Show help")
            ("get, g", "Get command")
            ("stop, s", "Stop client")
            ;

    descGet.add_options()
            ("name,n", po::value<std::string>(), "Get employer data by name")
            ("all, a", "Get all employers")
            ;

}

CmdParser::~CmdParser() {

}

void CmdParser::parse(const std::string &stream) {
    std::vector<std::string> cmds = tokenize(stream);
    po::options_description desc = descGeneral;
    po::parsed_options parsed = po::command_line_parser(cmds).options(desc).allow_unregistered().run();

    vm.clear();
    generalCmd = UNKNOWN_GENERAL_CMD;
    subCmd = UNKNOWN_SUB_CMD;
    po::store(parsed, vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        generalCmd = HELP;
    } else if (vm.count("get"))
    {
        generalCmd = GET;
        desc.add(descGet);
        po::store(po::command_line_parser(cmds).options(desc).run(), vm);
        if(vm.count("name"))
        {
            subCmd = NAME;
        } else if (vm.count("all"))
        {
            subCmd = ALL;
        }
    } else if (vm.count("stop"))
    {
        generalCmd = STOP;
    }

}

std::vector<std::string> CmdParser::tokenize(const std::string &input) {
    boost::escaped_list_separator<char> separator("\\",    // The escape characters.
                             "= ",    // The separator characters.
                             "\"\'" ); // The quote characters.

    boost::tokenizer<boost::escaped_list_separator<char>> tokens(input, separator);

    std::vector<std::string> result;
    copy_if(tokens.begin(), tokens.end(), std::back_inserter(result),
            !boost::bind(&std::string::empty, _1));
    return result;
}

const boost::program_options::variables_map &CmdParser::getVm() const {
    return vm;
}

GeneralCmd CmdParser::getGeneralCmd() const {
    return generalCmd;
}

SubCmd CmdParser::getSubCmd() const {
    return subCmd;
}

const boost::program_options::options_description &CmdParser::getDescGeneral() const {
    return descGeneral;
}

const boost::program_options::options_description &CmdParser::getDescGet() const {
    return descGet;
}
