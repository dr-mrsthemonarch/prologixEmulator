//
//  TCPSession.cpp
//
//
//  Created by drMrstheMonarch on 28.04.24.
//

#include "TCPSession.hpp"
#include <boost/bind/bind.hpp>

//// session class to allow multiple connections of clients separated from each other.
//TCPSession::TCPSession(ip::tcp::socket socket, Command& cmd, SharedVector& sharedVec)
//    : socket_(std::move(socket)), command_(cmd), sharedVec_(sharedVec) {}
//
//void TCPSession::start() {
//    read();
//}
//
//void TCPSession::read() {
//    auto self(shared_from_this());
//    socket_.async_read_some(buffer(data_),
//        [this, self](boost::system::error_code ec, std::size_t length) {
//            if (!ec) {
//                std::string received(data_.data(), length);
//                //std::cout << "Received: " << received << std::endl;
//                std::string response = command_.getResponse(received);
//                // Add received message to SharedVector
//                {
//                    std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
//                    sharedVec_.vec.push_back("Received: " + received);
//                }
//
//                write(response);
//            }
//        });
//}
//
//void TCPSession::write(const std::string& response) {
//    auto self(shared_from_this());
//    async_write(socket_, buffer(response),
//        [this, self, response](boost::system::error_code ec, std::size_t /*length*/) {
//            if (!ec) {
//                // Add sent message to SharedVector
//                {
//                    std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
//                    sharedVec_.vec.push_back("Sent: " + response);
//                }
//
//                read(); // Continue reading from the client
//            }
//        });
//}
TCPSession::TCPSession(ip::tcp::socket socket, Commander& cmd, SharedVector& sharedVec, SharedVector& clientVec)
    : socket_(std::move(socket)), command_(cmd), sharedVec_(sharedVec), clientVec_(clientVec), timer_(socket_.get_executor()) {
    // Retrieve and store the remote IP address
    remote_ip_ = socket_.remote_endpoint().address().to_string();
    add_connected_ip();
}

TCPSession::~TCPSession() {
    remove_connected_ip();
}

void TCPSession::start() {
    read();
    check_connection();  // Start periodic connection checks
}

//void TCPSession::read() {
//    auto self(shared_from_this());
//    socket_.async_read_some(boost::asio::buffer(data_),
//        [this, self](boost::system::error_code ec, std::size_t length) {
//            if (!ec) {
//                std::string received(data_.data(), length);
//                std::string response = command_.getResponse(received);
//                {
//                    std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
//                    sharedVec_.vec.push_back("Received: " + received);
//                }
//
//                write(response);
//            }
//        });
//}

void TCPSession::read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                std::string received(data_.data(), length);
                std::string commandName;
                std::vector<std::string> params;
                {
                    std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
                    sharedVec_.vec.push_back("Received: " + received);
                }
                if (Parser::parseCommand(received, commandName, params)) {
                    std::string response = Responder::getResponse(command_, commandName, params);
                    write(response);
                } else {
                    write("Invalid command format");
                }
            }
        });
}


void TCPSession::write(const std::string& response) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(response),
        [this, self, response](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                {
                    std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
                    sharedVec_.vec.push_back("Sent: " + response);
                }
                
                read(); // Continue reading from the client
            }
        });
}

void TCPSession::check_connection() {
    auto self(shared_from_this());
    timer_.expires_after(boost::asio::chrono::seconds(5));  // Check every 5 seconds
    timer_.async_wait([this, self](boost::system::error_code ec) {
        if (!ec) {
            // Use the socket's lowest layer to check if it's open
            if (socket_.is_open()) {
                // Schedule the next check
                std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
//                sharedVec_.vec.push_back("checking");
            } else {
                std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
                sharedVec_.vec.push_back(remote_ip_ + " disconnected.");
                // Update the list of connected IPs
                remove_connected_ip();
            }
        }
    });
}



void TCPSession::add_connected_ip() {
    std::lock_guard<std::mutex> clientlock(clientVec_.vecMutex);
    clientVec_.vec.push_back(remote_ip_);
    std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
    sharedVec_.vec.push_back("Added IP: " + remote_ip_ + ". Connected IPs: " + std::to_string(clientVec_.vec.size()));
}

void TCPSession::remove_connected_ip() {
    std::lock_guard<std::mutex> clientlock(clientVec_.vecMutex);
    auto it = std::find(clientVec_.vec.begin(), clientVec_.vec.end(), remote_ip_);
    if (it != clientVec_.vec.end()) {
        
        std::lock_guard<std::mutex> lock(sharedVec_.vecMutex);
        clientVec_.vec.erase(it);
        sharedVec_.vec.push_back("Removed IP: " + remote_ip_ + ". Connected IPs: " + std::to_string(clientVec_.vec.size()));

    }
}
