//
//  Responder.hpp
//  prologix_emulator
//
//  Created by Kyle on 09.06.24.
//

#ifndef Responder_hpp
#define Responder_hpp

#include "Functions.h"
#include "Commander.h"


class Responder {
public:
    static std::string getResponse(Commander& commander, const std::string& commandName, const std::vector<std::string>& params);
};

#endif /* Responder_hpp */
