////
////  command.cpp
////  supersimple
////
////  Created by Kyle on 28.04.24.
////
//
//#include "command.hpp"
////Command class to easily create commands for prologix command reponses.
///* Todo:
// add functionality for vset to store set values
// add function for vout that outputs stored value
// actually just re-read prologix documentation and implement each feature.
//
//*/
//    void Command::addCommand(const std::string& cmd, const std::string& response) {
//        commands_[cmd] = response;
//    }
//
//    std::string Command::getResponse(const std::string& cmd) {
//        auto it = commands_.find(cmd);
//        if (it != commands_.end()) {
//            return it->second;
//        } else {
//            return "Unknown command";
//        }
//    }
//
//    std::unordered_map<std::string, std::string> commands_;
//

#include "Command.hpp"
#include "functions.hpp"

Command::Command(SharedVector& sharedVec) : sharedVec_(sharedVec) {}

void Command::addCommand(const std::string& cmd, const std::string& response) {
    commands_[cmd] = response;
    
    // Add to SharedVector
    std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);

}
std::string Command::getResponse(const std::string& cmd) {
    // Remove the newline character if present
    std::string cleanedCmd = cmd;
    cleanedCmd = removeWhitespaces(cleanedCmd);
    std::string number;
    
    
    
    if (!cleanedCmd.empty() && cleanedCmd.back() == '\n') {
        cleanedCmd.pop_back();
    }
    std::string root = cleanStringAndGetNumber(cleanedCmd, number);
    
    
    
    // Find the index where the root starts (after number)
    size_t rootStart = cleanedCmd.find_first_not_of("0123456789");
    if (rootStart == std::string::npos)
        rootStart = 0;

    // Find the corresponding response for the root
    auto it = commands_.find(root);
    if (it != commands_.end()) {
        std::string response = it->second;

        // Append the number to the response if it exists
        if (!number.empty()) {
            response += " " + number;
        }

        // Add to SharedVector
        std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
        sharedVec_.vec.push_back("Command executed: " + cleanedCmd + " with response: " + response);
        return response;
    } else {
        // Add to SharedVector
        std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
        sharedVec_.vec.push_back("Unknown command executed: \n" + cleanedCmd);
        return "Unknown command";
    }
}









