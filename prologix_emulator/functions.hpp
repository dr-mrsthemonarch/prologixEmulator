//
//  functions.hpp
//  supersimple
//
//  Created by Kyle on 26.05.24.
//

#ifndef functions_hpp
#define functions_hpp

#include <stdio.h>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>
#include "ftxui/component/component.hpp"

struct SharedVector {
    std::vector<std::string> vec;
    std::mutex vecMutex;
};

boost::asio::ip::address_v6 sinaddr_to_asio(sockaddr_in6 *addr);

std::string addresses_to_string(const std::vector<boost::asio::ip::address>& addresses);

boost::asio::ip::address extract_valid_ip(const std::vector<boost::asio::ip::address>& addresses);

std::string extract_valid_ip_string(const std::vector<boost::asio::ip::address>& addresses);

std::vector<std::string> convertPacketToStrings(const std::array<char, 12>& header, const std::array<char, 64>& identify, bool useHex);

std::vector<std::string> split(const std::string &s, char delimiter);

std::string ipToHex(const std::string &ip);

std::array<unsigned char, 4> ipStringToByteArray(const std::string& ip);

std::vector<boost::asio::ip::address> get_local_interfaces();

ftxui::Component Wrap(std::string name, ftxui::Component component);

//std::vector<std::string> udpstart(bool batman, bool collect = true);
std::function<void()>udpstart(bool batman, bool collect = true);

std::string joinStrings(const std::vector<std::string>& strings);
std::string joinStrings(std::vector<std::string>&& strings);

std::string joinStringsAccumulate(const std::vector<std::string>& strings);
std::string joinStringsAccumulate(std::vector<std::string>&& strings);

std::string joinStringsStream(const std::vector<std::string>& strings);
std::string joinStringsStream(std::vector<std::string>&& strings) ;

void checkForNewEntry(SharedVector& sharedVec, size_t& lastSize,ftxui::Elements children);

void monitorVector(SharedVector& sharedVec,ftxui::Elements children);

std::string cleanStringAndGetNumber(std::string& input, std::string& number);

std::string removeWhitespaces(std::string& input);

#endif /* functions_hpp */
