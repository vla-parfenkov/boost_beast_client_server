//
// Created by vladislav on 26.03.19.
//

#ifndef SAMPLE_CLIENT_CMDPARSER_H
#define SAMPLE_CLIENT_CMDPARSER_H

#include <boost/program_options.hpp>

namespace po = boost::program_options;

enum GeneralCmd
{
    HELP,
    GET,
    STOP,
    UNKNOWN_GENERAL_CMD,
};

enum SubCmd
{
    NAME,
    ALL,
    UNKNOWN_SUB_CMD,
};

class CmdParser 
{
private:
    po::options_description descGeneral {"General option"};
    po::options_description descGet {"Get option"};
    po::variables_map vm;

    GeneralCmd generalCmd;
    SubCmd subCmd;

    std::vector<std::string> tokenize(const std::string& input);
public:

    CmdParser();
    ~CmdParser();

    void parse(const std::string& stream);

    const boost::program_options::variables_map &getVm() const;

    GeneralCmd getGeneralCmd() const;

    SubCmd getSubCmd() const;

    const boost::program_options::options_description &getDescGeneral() const;
    const boost::program_options::options_description &getDescGet() const;
};


#endif //SAMPLE_CLIENT_CMDPARSER_H
