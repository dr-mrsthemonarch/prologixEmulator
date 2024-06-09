//
//  Commander.cpp
//  
//
//  Created by drMrstheMonarch on 28.04.24.
//
#include "Commander.hpp"


void Commander::addCommand(const std::string& name, const std::string& defaultResponse, int numParams) {
        std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
        // Store the command in a vector with a maximum length of 2 + numParams
        std::vector<std::string> commandData(2 + numParams, "");
        commandData[0] = defaultResponse;
        commandData[1] = std::to_string(numParams);
        commands_[name] = commandData;
    }

const std::vector<std::string>* Commander::getCommand(const std::string& name) const {
    auto it = commands_.find(name);
    if (it != commands_.end()) {
        return &it->second;
    }
    return nullptr;
}

void Commander::setCommandParams(const std::string& name, const std::vector<std::string>& params) {
        std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
        auto it = commands_.find(name);
        if (it != commands_.end()) {
            // Update parameters starting from index 2
            int numParams = std::stoi(it->second[1]);
            for (size_t i = 0; i < std::min(params.size(), static_cast<size_t>(numParams)); ++i) {
                it->second[i + 2] = params[i];
            }
        }
    }

void Commander::resetAllCommandsToDefault() {
        std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
        for (auto& command : commands_) {
            // Reset parameters to default values starting from index 3
            std::fill(command.second.begin() + 2, command.second.end(), std::string(""));
        }
    }
