////
////  TCPServer.hpp
////  supersimple
////
////  Created by Kyle on 28.04.24.
////
//
//#ifndef TCPServer_hpp
//#define TCPServer_hpp

//#include <boost/asio.hpp>
//#include <vector>
//#include "command.hpp"
//#include "TCPSession.hpp"
//#include "functions.hpp"
//
//
//class TCPServer
//{
//public:
//    TCPServer(io_service& service, short port, Command& cmd);
//private:
//    void accept();
//    ip::tcp::acceptor acceptor_;
//    ip::tcp::socket socket_;
//    Command& command_;
//};
//
//
//#endif /* TCPServer_hpp */#ifndef TCPServer_hpp
#ifndef TCPServer_hpp
#define TCPServer_hpp

#include <boost/asio.hpp>
#include <vector>
#include "command.hpp"
#include "TCPSession.hpp"
#include "functions.hpp"
#include <memory>
#include <atomic>

class TCPServer
{
public:
        TCPServer(io_service& service, short port, Command& cmd, SharedVector& sharedVec);
//    TCPServer(boost::asio::io_service& service, short port, Command& cmd, SharedVector& sharedVec);
    void decreaseActiveSessions();
    
private:
    void accept();
    ip::tcp::socket socket_;
    SharedVector& sharedVec_; // Add SharedVector reference
    std::atomic<int> activeSessions_; // Counter for active sessions
    boost::asio::ip::tcp::acceptor acceptor_;
    Command& command_;
};

#endif /* TCPServer_hpp */

