//
//  Commander.hpp
//
//
//  Created by drMrstheMonarch on 28.04.24.
//
#ifndef COMMANDER_HPP
#define COMMANDER_HPP

#include <iostream>
#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include "functions.hpp"



class Commander {
public:
    struct Command {
        std::string defaultResponse;
        int numParams;
        std::vector<std::string> params;
    };
    Commander(SharedVector& sharedVec) : sharedVec_(sharedVec) {}

    void addCommand(const std::string& name, const std::string& defaultResponse, int numParams);
    
    const std::vector<std::string>* getCommand(const std::string& name) const;
    
    void setCommandParams(const std::string& name, const std::vector<std::string>& params);
    
    void resetAllCommandsToDefault();
    

private:
    SharedVector& sharedVec_;
    std::unordered_map<std::string, std::vector<std::string>> commands_;
};

#endif // COMMANDER_HPP
