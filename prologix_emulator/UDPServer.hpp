//
//  UDPServer.hpp
//
//
//  Created by drMrstheMonarch on 28.04.24.
//
#ifndef UDPServer_hpp
#define UDPServer_hpp
#include <boost/asio.hpp>
#include <array>
#include <memory>
#include "Functions.hpp"
using boost::asio::ip::udp;


class UDPServer {
public:
    UDPServer(boost::asio::io_context& io_context, short port,SharedVector& sharedVec);
    std::vector<std::string> collectEntries();
    std::vector<std::string> getEntries();
    
private:
    void start_receive(SharedVector& sharedVec);
    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred,SharedVector& sharedVec);
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    std::array<char, 1024> recv_buffer_;
    std::array<char, 64> header;
//    std::vector<std::string> udp_entries;
    SharedVector udp_entries;
};

#endif /* UDPServer_hpp */
