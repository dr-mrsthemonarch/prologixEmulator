////
////  command.hpp
////  supersimple
////
////  Created by Kyle on 28.04.24.
////
//
//#ifndef command_hpp
//#define command_hpp
//
//
//#include <boost/asio.hpp>
//#include <memory>
//#include <unordered_map>
//
//class Command
//{
//private:
//    std::unordered_map<std::string, std::string> commands_;
//public:
//    Command() {};
//    void addCommand(const std::string& cmd, const std::string& response);
//    std::string getResponse(const std::string& cmd);
//};
//
//
//#endif /* command_hpp */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "functions.hpp"

class Command {
public:
    Command(SharedVector& sharedVec);

    void addCommand(const std::string& cmd, const std::string& response);
    std::string getResponse(const std::string& cmd);

private:
    std::unordered_map<std::string, std::string> commands_;
    SharedVector& sharedVec_;
};

#endif // COMMAND_HPP
