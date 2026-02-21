//
//  Responder.cpp
//  prologix_emulator
//
//  Created by Kyle on 09.06.24.
//

#include "Responder.h"

// Set of commands whose numeric responses are subject to polarity
static const std::unordered_set<std::string> polarity_commands = {
    "VOUT?", "VSET", "HVON", "HVOF"
};

static std::string applyPolarity(const std::string& commandName, const std::string& response) {
    if (polarity_commands.find(commandName) == polarity_commands.end()) return response;
    try {
        double val = std::stod(response);
        double abs_val = std::abs(val);
        if (polarity_positive.load()) {
            // preserve integer formatting
            if (response.find('.') == std::string::npos)
                return "+" + std::to_string(static_cast<long long>(abs_val));
            return "+" + std::to_string(abs_val);
        } else {
            if (response.find('.') == std::string::npos)
                return "-" + std::to_string(static_cast<long long>(abs_val));
            return "-" + std::to_string(abs_val);
        }
    } catch (...) {
        return response;
    }
}


std::string Responder::getResponse(Commander& commander, const std::string& commandName, const std::vector<std::string>& params) {
    if (commandName == "++reset") {
        commander.resetAllCommandsToDefault();
        return "All commands reset to default values";
    }

    const std::vector<std::string>* commandData = commander.getCommand(commandName);
    if (!commandData) {
        return "Unknown command";
    }

    if (commandName == "VOUT?") {
        commandData = commander.getCommand("VSET"); //temp solution until direct hardware is developed.
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

    return applyPolarity(commandName, response);
};