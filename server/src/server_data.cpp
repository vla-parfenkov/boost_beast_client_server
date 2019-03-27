//
// Created by vladislav on 19.03.19.
//

#include <fstream>
#include "server_data.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <iostream>


ServerData::ServerData() {

}

ServerData::~ServerData() {

}

void ServerData::readDataFromFile(const std::string &path)
{
    std::ifstream in(path);

    if(!in.good())
    {
        std::cerr << path << " file not found" << std::endl;
        return;
    }
    try
    {
        boost::property_tree::read_json(in, employers);

    } catch (const boost::property_tree::json_parser_error& error) {
        std::cerr << error.what() << std::endl;
        return;
    }

}

std::string ServerData::getAllEmployersAsString()
{
    std::stringstream stream;

    BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, employers.get_child("data"))
    {
        stream << "\"" << v.second.get_child("name").data() << "\"" << ",";
    }

    std::string str = stream.str();
    str.pop_back();
    return str;
}

std::string ServerData::getEmployeeByNameAsJsonString(const std::string &name)
{
    std::stringstream stream;

    std::cout << "find " << name << " from server.json" << std::endl;

    BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, employers.get_child("data"))
    {
        if(v.second.get_child("name").data() == name)
        {
            boost::property_tree::write_json(stream, v.second);
            return stream.str();
        }
    }

    throw std::invalid_argument("Employers Noy found");
}
