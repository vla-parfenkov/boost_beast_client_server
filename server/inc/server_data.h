//
// Created by vladislav on 19.03.19.
//

#ifndef SAMPLE_SERVER_SERVER_DATA_H
#define SAMPLE_SERVER_SERVER_DATA_H

#include <string>
#include <map>
#include <boost/property_tree/ptree.hpp>



class ServerData 
{
private:
    struct  Employee
    {
        std::string name;
        size_t age;
        std::string phone;
    };

    boost::property_tree::ptree employers;

public:

    ServerData();
    ~ServerData();

    void readDataFromFile(const std::string& path);

    std::string getAllEmployersAsString();
    std::string getEmployeeByNameAsJsonString(const std::string& name);

};

#endif //SAMPLE_SERVER_SERVER_DATA_H
