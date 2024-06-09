//
//  Parser.hpp
//  prologix_emulator
//
//  Created by Kyle on 09.06.24.
//

#ifndef Parser_hpp
#define Parser_hpp

#include "Functions.hpp"
#include "Commander.hpp"


class Parser {
public:
    static bool parseCommand(const std::string& input, std::string& commandName, std::vector<std::string>& params);
};
#endif /* Parser_hpp */
