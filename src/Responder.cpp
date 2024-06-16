//
//  Responder.cpp
//  prologix_emulator
//
//  Created by Kyle on 09.06.24.
//

#include "Responder.h"


std::string Responder::getResponse(Commander& commander, const std::string& commandName, const std::vector<std::string>& params) {
    if (commandName == "++reset") {
        commander.resetAllCommandsToDefault();
        return "All commands reset to default values";
    }

    const std::vector<std::string>* commandData = commander.getCommand(commandName);
    if (!commandData) {
        return "Unknown command";
    }

    int numParams = std::stoi((*commandData)[1]);
    if (params.size() <= numParams) {
        commander.setCommandParams(commandName, params); // Update parameters
    } else {
        return "Invalid number of parameters";
    }

    std::string response;

    if (std::stoi((*commandData)[1])==0) {
        response = (*commandData)[0];
    } else if ( (*commandData)[2].empty() ) {
        response = (*commandData)[0];
    } else {
        for (unsigned long i = 2; i < commandData->size(); ++i) {
            response += " " + (*commandData)[i];
        }
    }

    return response;
};
