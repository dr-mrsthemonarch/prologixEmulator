//
//  TCPSession.hpp
//
//
//  Created by drMrstheMonarch on 28.04.24.
//
#ifndef TCPSession_hpp
#define TCPSession_hpp


#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Commander.hpp"
#include "Parser.hpp"
#include "Responder.hpp"
#include "Functions.hpp" // Include for SharedVector

using namespace boost::asio;


class TCPSession : public std::enable_shared_from_this<TCPSession>
{
public:
    TCPSession(ip::tcp::socket socket, Commander& cmd, SharedVector& sharedVec,SharedVector& clientVec);
    ~TCPSession();
    
    void start();
    
private:
    void read();
    void write(const std::string& response);
    void check_connection();
    void add_connected_ip();
    void remove_connected_ip();
    
    ip::tcp::socket socket_;
    Commander& command_;
    SharedVector& sharedVec_;
    SharedVector& clientVec_;
    std::array<char, 1024> data_;
    boost::asio::steady_timer timer_;  // Timer for periodic connection checks
    std::string remote_ip_;  // Remote IP address
};

#endif /* TCPSession_hpp */
