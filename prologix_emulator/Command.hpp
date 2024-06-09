//
//  command.hpp
//
//
//  Created by drMrstheMonarch on 28.04.24.
//
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "Functions.hpp"

class Command {
 public:
  Command(SharedVector& sharedVec);

  void addCommand(const std::string& cmd, const std::string& response);
  std::string getResponse(const std::string& cmd);

 private:
  std::unordered_map<std::string, std::string> commands_;
  SharedVector& sharedVec_;
};

#endif  // COMMAND_HPP
