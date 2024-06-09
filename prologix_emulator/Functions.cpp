//
//  functions.cpp
//
//
//  Created by drMrstheMonarch on 28.04.24.
//

#include "Functions.hpp"
#include "UDPServer.hpp"
#include <string>
#include <vector>
#include <numeric>
#include <sstream>
#include <mutex>
#include <cstring>

using namespace ftxui;

Component Wrap(std::string name, Component component) {
    return Renderer(component, [name, component] {
        return hbox({
            text(name) | size(WIDTH, EQUAL, 15),
            separator() |  color(Color::Default),
            component->Render() | xflex ,
        });
    });
}

// Method 1: Using a simple loop
// Overload for const reference
std::string joinStrings(const std::vector<std::string>& strings) {
    std::string result;
    for (const auto& str : strings) {
        result += str;
    }
    return result;
}

// Overload for rvalue reference
std::string joinStrings(std::vector<std::string>&& strings) {
    std::string result;
    for (auto& str : strings) {
        result += std::move(str);
    }
    return result;
}

// Method 2: Using std::accumulate
// Overload for const reference
std::string joinStringsAccumulate(const std::vector<std::string>& strings) {
    return std::accumulate(strings.begin(), strings.end(), std::string());
}

// Overload for rvalue reference
std::string joinStringsAccumulate(std::vector<std::string>&& strings) {
    return std::accumulate(
        std::make_move_iterator(strings.begin()),
        std::make_move_iterator(strings.end()),
        std::string()
    );
}

// Method 3: Using std::ostringstream
// Overload for const reference
std::string joinStringsStream(const std::vector<std::string>& strings) {
    std::ostringstream oss;
    for (const auto& str : strings) {
        oss << str;
    }
    return oss.str();
}

// Overload for rvalue reference
std::string joinStringsStream(std::vector<std::string>&& strings) {
    std::ostringstream oss;
    for (auto& str : strings) {
        oss << std::move(str);
    }
    return oss.str();
}


boost::asio::ip::address_v6 sinaddr_to_asio(sockaddr_in6 *addr) {
    boost::asio::ip::address_v6::bytes_type buf;
    memcpy(buf.data(), addr->sin6_addr.s6_addr, sizeof(addr->sin6_addr));
    return boost::asio::ip::make_address_v6(buf, addr->sin6_scope_id);
}

std::string addresses_to_string(const std::vector<boost::asio::ip::address>& addresses) {
    std::string result;
    for (const auto& address : addresses) {
        result += address.to_string() + "\n";
    }
    return result;
}
// Function to extract a valid IPv4 address as a boost::asio::ip::address
boost::asio::ip::address extract_valid_ip(const std::vector<boost::asio::ip::address>& addresses) {
    for (const auto& address : addresses) {
        if (!address.is_v4()) continue; // Skip non-IPv4 addresses
        std::string ip_string = address.to_string();
        if (ip_string.substr(0, 4) != "127.") { // Exclude addresses starting with "127."
            return address;
        }
    }
    // If no valid address found, return a loopback address
    return boost::asio::ip::address::from_string("127.0.0.1");
}

// Overloaded function to extract a valid IPv4 address as a std::string
std::string extract_valid_ip_string(const std::vector<boost::asio::ip::address>& addresses) {
    for (const auto& address : addresses) {
        if (!address.is_v4()) continue; // Skip non-IPv4 addresses
        std::string ip_string = address.to_string();
        if (ip_string.substr(0, 4) != "127.") { // Exclude addresses starting with "127."
            return ip_string;
        }
    }
    // If no valid address found, return the loopback address as a string
    return "127.0.0.1";
}

std::vector<std::string> convertPacketToStrings(const std::array<char, 12>& header, const std::array<char, 64>& identify, bool useHex = true) {
    std::vector<std::string> packetStrings;
    
    // Convert header array to string
    std::stringstream headerStream;
    for (size_t i = 0; i < header.size(); ++i) {
        if (useHex) {
            headerStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(header[i]));
        } else {
            headerStream << static_cast<int>(static_cast<unsigned char>(header[i]));
        }
        if (i < header.size() - 1) {
            headerStream << " "; // Add space between bytes
        }
    }
    packetStrings.push_back(headerStream.str());
    
    // Convert identify array to string
    std::stringstream identifyStream;
    for (size_t i = 0; i < identify.size(); ++i) {
        if (useHex) {
            identifyStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(identify[i]));
        } else {
            identifyStream << static_cast<int>(static_cast<unsigned char>(identify[i]));
        }
        if (i < identify.size() - 1) {
            identifyStream << " "; // Add space between bytes
        }
    }
    packetStrings.push_back(identifyStream.str());
    
    return packetStrings;
}

// Function to split a string by a delimiter
std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to convert an IP address to an ASCII hexadecimal format
std::string ipToHex(const std::string &ip) {
    std::vector<std::string> octets = split(ip, '.');
    std::ostringstream hexStream;
    hexStream << std::hex << std::setfill('0');
    hexStream << "\""; // Add opening quote
    for (const auto& octet : octets) {
        int num = std::stoi(octet);
        hexStream << "\\x" << std::setw(2) << num;
    }
    hexStream << "\""; // Add closing quote
    return hexStream.str();
}

// Function to convert an IPv4 address string to a byte array
std::array<unsigned char, 4> ipStringToByteArray(const std::string& ip) {
    std::array<unsigned char, 4> bytes;
    std::stringstream ss(ip);
    int a, b, c, d;
    char dot;
    ss >> a >> dot >> b >> dot >> c >> dot >> d;
    bytes[0] = static_cast<unsigned char>(a);
    bytes[1] = static_cast<unsigned char>(b);
    bytes[2] = static_cast<unsigned char>(c);
    bytes[3] = static_cast<unsigned char>(d);
    return bytes;
}


//find local interface IP, returns a vector of ips
#if defined(_WIN32)
#undef UNICODE
#include <winsock2.h>
// Headers that need to be included after winsock2.h:
#include <iphlpapi.h>
#include <ws2ipdef.h>
// https://stackoverflow.com/a/62303963/15907933
typedef IP_ADAPTER_UNICAST_ADDRESS_LH Addr;
typedef IP_ADAPTER_ADDRESSES *AddrList;

std::vector<boost::asio::ip::address> get_local_interfaces() {
    // It's a windows machine, we assume it has 512KB free memory
    DWORD outBufLen = 1 << 19;
    AddrList ifaddrs = (AddrList) new char[outBufLen];
    
    std::vector<boost::asio::ip::address> res;
    
    ULONG err = GetAdaptersAddresses(AF_UNSPEC,
                                     GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_DNS_SERVER, NULL, ifaddrs,
                                     &outBufLen);
    
    if (err == NO_ERROR) {
        for (AddrList addr = ifaddrs; addr != 0; addr = addr->Next) {
            if (addr->OperStatus != IfOperStatusUp) continue;
            // if (addr->NoMulticast) continue;
            
            // Find the first IPv4 address
            if (addr->Ipv4Enabled) {
                for (Addr *uaddr = addr->FirstUnicastAddress; uaddr != 0; uaddr = uaddr->Next) {
                    if (uaddr->Address.lpSockaddr->sa_family != AF_INET) continue;
                    res.push_back(boost::asio::ip::make_address_v4(ntohl(reinterpret_cast<sockaddr_in *>(addr->ifa_addr)->sin_addr.s_addr)));
                }
            }
            
            if (addr->Ipv6Enabled) {
                for (Addr *uaddr = addr->FirstUnicastAddress; uaddr != 0; uaddr = uaddr->Next) {
                    if (uaddr->Address.lpSockaddr->sa_family != AF_INET6) continue;
                    res.push_back(sinaddr_to_asio(reinterpret_cast<sockaddr_in6 *>(addr->ifa_addr)));
                }
            }
        }
    } else {
        
    }
    delete[]((char *)ifaddrs);
    return res;
}
#elif defined(__APPLE__) || defined(__linux__)
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/types.h>

std::vector<boost::asio::ip::address> get_local_interfaces() {
    std::vector<boost::asio::ip::address> res;
    ifaddrs *ifs;
    if (getifaddrs(&ifs)) {
        return res;
    }
    for (auto addr = ifs; addr != nullptr; addr = addr->ifa_next) {
        // No address? Skip.
        if (addr->ifa_addr == nullptr) continue;
        
        // Interface isn't active? Skip.
        if (!(addr->ifa_flags & IFF_UP)) continue;
        
        if(addr->ifa_addr->sa_family == AF_INET) {
            res.push_back(boost::asio::ip::make_address_v4(ntohl(reinterpret_cast<sockaddr_in *>(addr->ifa_addr)->sin_addr.s_addr)));
        } else if(addr->ifa_addr->sa_family == AF_INET6) {
            res.push_back(sinaddr_to_asio(reinterpret_cast<sockaddr_in6 *>(addr->ifa_addr)));
        } else continue;
    }
    freeifaddrs(ifs);
    return res;
}
#else
#error "..."
#endif

// Function to check if the vector has a new entry
void checkForNewEntry(SharedVector& sharedVec, unsigned long& lastSize,Elements children) {
    std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
    if (sharedVec.vec.size() > lastSize) {
        lastSize = sharedVec.vec.size();
        //return sharedVec.vec.back();
        }
    }

// Function to monitor the vector for new entries
void monitorVector(SharedVector& sharedVec,Elements children) {
    size_t lastSize = 0;
    while (true) {
        checkForNewEntry(sharedVec, lastSize,children);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep for 100 ms before next check
    }
}


std::string cleanStringAndGetNumber(std::string& input, std::string& number) {
    // Remove the newline character if present
    if (!input.empty() && input.back() == '\n') {
        input.pop_back();
    }

    // Initialize variables to store cleaned string and extracted number
    std::string cleanedString;
    number.clear();

    // Iterate through each character in the input string
    for (char c : input) {
        // Check if the character is a digit
        if (std::isdigit(c)) {
            // Append the digit to the number string
            number += c;
        } else {
            // Append non-digit characters to the cleaned string
            cleanedString += c;
        }
    }

    // Return the cleaned string
    return cleanedString;
}

std::string removeWhitespaces(std::string& input) {
    // Use std::remove_if algorithm to remove whitespace characters
    input.erase(std::remove_if(input.begin(), input.end(),
                               [](unsigned char c) { return std::isspace(c); }),
                input.end());

    // Return the modified string
    return input;
}

//==============================================================================================================================
