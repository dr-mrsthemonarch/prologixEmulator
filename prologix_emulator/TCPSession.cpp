//
//  TCPSession.cpp
//
//
//  Created by drMrstheMonarch on 28.04.24.
//

#include "TCPSession.hpp"
#include <boost/bind/bind.hpp>

// session class to allow multiple connections of clients separated from each other.
TCPSession::TCPSession(ip::tcp::socket socket, Command& cmd, SharedVector& sharedVec)
    : socket_(std::move(socket)), command_(cmd), sharedVec_(sharedVec) {}

void TCPSession::start() {
    read();
}

void TCPSession::read() {
    auto self(shared_from_this());
    socket_.async_read_some(buffer(data_),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                std::string received(data_.data(), length);
                //std::cout << "Received: " << received << std::endl;
                std::string response = command_.getResponse(received);
                // Add received message to SharedVector
                {
                    std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
                    sharedVec_.vec.push_back("Received: " + received);
                }
                
                write(response);
            }
        });
}

void TCPSession::write(const std::string& response) {
    auto self(shared_from_this());
    async_write(socket_, buffer(response),
        [this, self, response](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                // Add sent message to SharedVector
                {
                    std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
                    sharedVec_.vec.push_back("Sent: " + response);
                }
                
                read(); // Continue reading from the client
            }
        });
}

