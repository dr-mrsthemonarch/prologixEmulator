////
////  TCPSession.hpp
////  supersimple
////
////  Created by Kyle on 28.04.24.
////
//
//#ifndef TCPSession_hpp
//#define TCPSession_hpp

//#include <stdio.h>
//#include <iostream>
//#include <boost/asio.hpp>
//#include <vector>
//#include <memory>
//#include <unordered_map>
//#include "command.hpp"
//
//using namespace boost::asio;
//
//class TCPSession : public std::enable_shared_from_this<TCPSession>
//{
//public:
//    TCPSession(ip::tcp::socket socket, Command& cmd);
//    void start();
//private:
//    void read();
//    void write(const std::string& response);
//    //these three arn't added to the cpp part of the class.
//    ip::tcp::socket socket_;
//    std::array<char, 1024> data_;
//    Command& command_;
//};
//
//
//
//#endif /* TCPSession_hpp */

#ifndef TCPSession_hpp
#define TCPSession_hpp

#include <stdio.h>
#include <iostream>
#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <unordered_map>
#include "command.hpp"
#include "functions.hpp" // Include for SharedVector

using namespace boost::asio;

class TCPSession : public std::enable_shared_from_this<TCPSession>
{
public:
    TCPSession(ip::tcp::socket socket, Command& cmd, SharedVector& sharedVec);
    void start();
private:
    void read();
    void write(const std::string& response);
    ip::tcp::socket socket_;
    std::array<char, 1024> data_;
    Command& command_;
    SharedVector& sharedVec_; // Add SharedVector reference
};

#endif /* TCPSession_hpp */
