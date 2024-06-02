////
////  TCPServer.cpp
////  supersimple
////
////  Created by Kyle on 28.04.24.
////
//
//#include "TCPServer.hpp"
//#include "functions.hpp"
////Server class to start the server.
//
//
//    TCPServer::TCPServer(io_service& service, short port, Command& cmd)
//        : acceptor_(service, ip::tcp::endpoint(ip::tcp::v4(), port)),
//          socket_(service),
//          command_(cmd) {
//        accept();
//    }
//
//
//    void TCPServer::accept() {
//        acceptor_.async_accept(socket_,
//            [this](boost::system::error_code ec) {
//                if (!ec) {
//                    std::cout << "Connection established." << std::endl;
//                    std::make_shared<TCPSession>(std::move(socket_), command_)->start(); // Start a new session
//                }
//                accept(); // Accept new connections
//            });
//    }
//

#include "TCPServer.hpp"
#include "functions.hpp"

//Server class to start the server.
TCPServer::TCPServer(io_service& service, short port, Command& cmd, SharedVector& sharedVec)
    : acceptor_(service, ip::tcp::endpoint(ip::tcp::v4(), port)),
      socket_(service),
      command_(cmd),
      sharedVec_(sharedVec) { // Initialize SharedVector
    accept();
}

void TCPServer::accept() {
    acceptor_.async_accept(socket_,
        [this](boost::system::error_code ec) {
            if (!ec) {
                std::cout << "Connection established." << std::endl;
                std::make_shared<TCPSession>(std::move(socket_), command_, sharedVec_)->start(); // Pass SharedVector
            }
            accept(); // Accept new connections
        });
}
