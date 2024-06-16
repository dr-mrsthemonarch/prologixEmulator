//
//  Parser.cpp
//  prologix_emulator
//
//  Created by Kyle on 09.06.24.
//

#include "Parser.h"
#include "Commander.h"


bool Parser::parseCommand(const std::string& input, std::string& commandName, std::vector<std::string>& params) {
    std::istringstream iss(input);
    iss >> commandName;

    std::string param;
    while (iss >> param) {
        params.push_back(param);
    }

    return !commandName.empty();
}
