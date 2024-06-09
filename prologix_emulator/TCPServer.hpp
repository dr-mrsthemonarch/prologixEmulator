//
//  TCPServer.hpp
//
//
//  Created by drMrstheMonarch on 28.04.24.
//
#ifndef TCPServer_hpp
#define TCPServer_hpp

#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <atomic>
#include "Commander.hpp"
#include "TCPSession.hpp"
#include "Functions.hpp"


class TCPServer
{
public:
    
    TCPServer(boost::asio::io_service& service, short port, Commander& cmd, SharedVector& sharedVec, SharedVector& clientVec);
    
private:
    void accept();
    ip::tcp::socket socket_;
    SharedVector& sharedVec_; // Add SharedVector reference
    SharedVector& clientVec_;
    std::atomic<int> activeSessions_; // Counter for active sessions
    boost::asio::ip::tcp::acceptor acceptor_;
    Commander& command_;
};

#endif /* TCPServer_hpp */
