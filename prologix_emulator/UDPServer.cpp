//
//  UDPServer.cpp
//
//
//  Created by drMrstheMonarch on 28.04.24.
//
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, operator|, text, center, border
#include <sstream>
#include <iomanip>
#include <vector>
#include <array>
#include <string>
#include "functions.hpp"
#include "UDPServer.hpp"
using boost::asio::ip::udp;

UDPServer::UDPServer(boost::asio::io_context& io_context, short port,SharedVector& sharedVec)
: socket_(io_context, udp::endpoint(udp::v4(), port)) {
    start_receive(sharedVec);
}

void UDPServer::start_receive(SharedVector& sharedVec) {
    socket_.async_receive_from(boost::asio::buffer(recv_buffer_),
                               remote_endpoint_,boost::bind(&UDPServer::handle_receive, this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,boost::ref(sharedVec)));
}

void UDPServer::handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred,SharedVector& sharedVec) {
    
    auto vector_ip = get_local_interfaces(); // get local ip
    std::string extracted_ip = extract_valid_ip_string(vector_ip); //vector_ip is a list of all address at the interface, return only the valid one.
    
    // Convert the extracted IP address string to a byte array
    std::array<unsigned char, 4> ip_bytes = ipStringToByteArray(extracted_ip);
    
    if (!error || error == boost::asio::error::message_size) {
        std::string hostname = remote_endpoint_.address().to_string();
        unsigned short port = remote_endpoint_.port();
        //std::cout << "Received identify from " << hostname << "/" << port << std::endl;
        
        uint16_t seq;
        std::memcpy(&seq, recv_buffer_.data() + 2, sizeof(seq));
        
        //create the discovery packet 
        // Construct header
        std::array<char, 12> header;
        header[0] = 0x5A; // magic
        header[1] = 1;    // identify reply
        std::memcpy(header.data() + 2, &seq, sizeof(seq));
        std::memcpy(header.data() + 4, "\x00\x24\x1d\x8b\x0e\x50", 6); // eth_addr
        //https://stackoverflow.com/questions/78514774/modifying-udp-perl-code-to-modern-boost-errors-in-packet
        // Construct identify reply
        std::array<char, 64> identify_reply;
        std::memset(identify_reply.data(), 0, 4); // uptime needs to be fixed.
        identify_reply[4] = 1; // mode
        identify_reply[5] = 0; // alert
        identify_reply[6] = 4; // ip type
        std::memcpy(identify_reply.data() + 8, ip_bytes.data(), 4); // ip addr: 10.10.10.10
        std::memcpy(identify_reply.data() + 12, "\xff\xff\xff\x00", 4); // netmask
        std::memcpy(identify_reply.data() + 16, "\x0a\x0a\x0a\x01", 4); // gateway
        std::memcpy(identify_reply.data() + 20, "\x01\x02\x03\x04", 4); // app ver
        std::memcpy(identify_reply.data() + 24, "\x01\x02\x03\x04", 4); // boot ver
        std::memcpy(identify_reply.data() + 28, "\x01\x02\x03\x04", 4); // hw ver
        std::memcpy(identify_reply.data() + 32, "PL ETH/USB", 10); // name
        
        // Convert header and identify reply to vector of chars
        std::vector<char> send_buffer(header.begin(), header.end());
        send_buffer.insert(send_buffer.end(), identify_reply.begin(), identify_reply.end());
        // Convert packet to vector of strings
        std::vector<std::string> packetStrings = convertPacketToStrings(header, identify_reply,false);
        
        // Send packet
        socket_.send_to(boost::asio::buffer(send_buffer), remote_endpoint_);
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(joinStrings(packetStrings));
        std::stringstream ss;
        ss <<"Received identify from " << &hostname << "/" << &port;
        sharedVec.vec.push_back(ss.str());
        // Start receiving again
        start_receive(sharedVec);
    }
}

std::vector<std::string> UDPServer::collectEntries() {
    // Return the collected entries
    return udp_entries.vec;
}

// Function to retrieve the collected entries
std::vector<std::string> UDPServer::getEntries() {
    return udp_entries.vec;
}
