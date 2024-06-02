std::string server_control(boost::asio::io_context& io_context,std::unique_ptr<std::thread>& io_thread,std::unique_ptr<UDPServer>& udp_server,std::unique_ptr<TCPServer>& tcp_server,const std::string& command,SharedVector& sharedVec,Command& cmd,const std::string& server_type) {
    std::string returner;
    
    if (command == "start") {
        if (!joker.load()) {
            joker.store(true);
            if (server_type == "UDP") {
                // Create a new UDP server instance and run it in a separate thread
                udp_server = std::make_unique<UDPServer>(io_context, 3040, sharedVec);
            } else if (server_type == "TCP") {
                // Create a new TCP server instance and run it in a separate thread
                tcp_server = std::make_unique<TCPServer>(io_context, 1234, cmd, sharedVec);
            }

            io_thread = std::make_unique<std::thread>([&io_context]() {
                io_context.run();
            });
            returner = "Starting " + server_type + " server...";
        }
    } else if (command == "stop") {
        if (joker.load()) {
            joker.store(false);
            // Stop the io_context and wait for the thread to finish
            io_context.stop();
            if (io_thread && io_thread->joinable()) {
                io_thread->join();
            }
            
            // Reset io_context and delete the server instances
            io_context.restart();
            if (server_type == "UDP") {
                udp_server.reset();
            } else if (server_type == "TCP") {
                tcp_server.reset();
            }
            
            returner = server_type + " Server stopped.";
        } else {
            returner = server_type + " Server is not running.";
        }
    }else if (command == "exit") {
                if (joker.load()) {
                    joker.store(false);
                    io_context.stop();
                    if (io_thread && io_thread->joinable()) {
                        io_thread->join();
                    }
                    io_context.restart();
                    udp_server.reset();
                    tcp_server.reset();
                }
                returner = "Exiting...";
            }
    
    return returner;
}


#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, operator|, text, center, border
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <array>
#include <memory>  // for shared_ptr, allocator, __shared_ptr_access
#include "UDPServer.hpp"
#include "functions.hpp"
#include "TCPServer.hpp"
#include "command.hpp"

using boost::asio::ip::udp;
using namespace ftxui;
std::atomic<bool> batman(false); // Use atomic for thread-safe boolean
std::atomic<bool> joker(false); // Use atomic for thread-safe boolean


// Function to control the server
std::string udp_server(boost::asio::io_context& io_context, std::unique_ptr<std::thread>& io_thread, std::unique_ptr<UDPServer>& server,std::string command,SharedVector& sharedVec) {
    std::string returner;
    std::vector<std::string> output;
    if (command == "start") {
        if (!batman.load()) {
            batman.store(true);
            // Create a new server instance and run it in a separate thread
            server = std::make_unique<UDPServer>(io_context, 3040,sharedVec);
            
            io_thread = std::make_unique<std::thread>([&io_context]() {
                io_context.run();
            });
            returner = "Starting server...";
            //output = server->collectEntries();
        }
    } else if (command == "stop") {
        if (batman.load()) {
            batman.store(false);
            // Stop the io_context and wait for the thread to finish
            io_context.stop();
            if (io_thread && io_thread->joinable()) {
                io_thread->join();
            }
            
            // Reset io_context and delete the server instance
            io_context.restart();
            server.reset();
            //return a response
            returner = "Server stopped.";
        } else {
            returner = "Server is not running.";
        }
    }
    //return {returner,output};
    return returner;
}

//// Function to control the server
//std::string tcp_control(boost::asio::io_service& io_service,std::unique_ptr<TCPServer>& server,std::string command,SharedVector& sharedVec) {
//    std::string returner;
//    std::vector<std::string> output;
//    if (command == "start") {
//        if (!joker.load()) {
//            joker.store(true);
//            // Create a new server instance and run it in a separate thread
//            server = std::make_unique<TCPServer>(service, 1234, cmd, sharedVec);
//
//            io_thread = std::make_unique<std::thread>([&io_context]() {
//                io_context.run();
//            });
//            returner = "Starting server...";
//            //output = server->collectEntries();
//        } else {
//            returner = "Server is already running.";
//        }
//    } else if (command == "stop") {
//        if (joker.load()) {
//            joker.store(false);
//            // Stop the io_context and wait for the thread to finish
//            io_context.stop();
//            if (io_thread && io_thread->joinable()) {
//                io_thread->join();
//            }
//            // Reset io_context and delete the server instance
//            io_context.restart();
//            server.reset();
//            //return a response
//            returner = "Server stopped.";
//        } else {
//            returner = "Server is not running.";
//        }
//    } else if (command == "exit") {
//        if (joker.load()) {
//            joker.store(false);
//            io_context.stop();
//            if (io_thread && io_thread->joinable()) {
//                io_thread->join();
//            }
//            io_context.restart();
//            server.reset();
//        }
//        returner = "Exiting...";
//    }
//    return returner;
//}
// Function to control the TCPServer
std::string tcp_control(boost::asio::io_context& io_context,
                        std::unique_ptr<std::thread>& io_thread,std::unique_ptr<TCPServer>& tcp_server,const std::string& command,SharedVector& sharedVec,Command& cmd) {
    std::string returner;
    
    if (command == "start") {
        if (!joker.load()) {
            joker.store(true);
            // Create a new TCP server instance and run it in a separate thread
            tcp_server = std::make_unique<TCPServer>(io_context, 1234, cmd, sharedVec);
            
            io_thread = std::make_unique<std::thread>([&io_context]() {
                io_context.run();
            });
            returner = "Starting TCP server...";
        }
    } else if (command == "stop") {
        if (joker.load()) {
            joker.store(false);
            // Stop the io_context and wait for the thread to finish
            io_context.stop();
            if (io_thread && io_thread->joinable()) {
                io_thread->join();
            }
            
            // Reset io_context and delete the server instance
            io_context.restart();
            tcp_server.reset();
            
            returner = "TCP Server stopped.";
        } else {
            returner = "TCP Server is not running.";
        }
    }
    
    return returner;
}
//// Function to control the servers
//std::string server_control(boost::asio::io_context& io_context,std::unique_ptr<std::thread>& io_thread,std::unique_ptr<UDPServer>& udp_server,std::unique_ptr<TCPServer>& tcp_server,const std::string& command,SharedVector& sharedVec,Command& cmd,const std::string& server_type) {
//    std::string returner;
//
//    if (command == "start") {
//        if (!joker.load()) {
//            joker.store(true);
//            if (server_type == "UDP") {
//                // Create a new UDP server instance and run it in a separate thread
//                udp_server = std::make_unique<UDPServer>(io_context, 3040, sharedVec);
//            } else if (server_type == "TCP") {
//                // Create a new TCP server instance and run it in a separate thread
//                tcp_server = std::make_unique<TCPServer>(io_context, 1234, cmd, sharedVec);
//            }
//
//            io_thread = std::make_unique<std::thread>([&io_context]() {
//                io_context.run();
//            });
//            returner = "Starting " + server_type + " server...";
//        }
//    } else if (command == "stop") {
//        if (joker.load()) {
//            joker.store(false);
//            // Stop the io_context and wait for the thread to finish
//            io_context.stop();
//            if (io_thread && io_thread->joinable()) {
//                io_thread->join();
//            }
//
//            // Reset io_context and delete the server instances
//            io_context.restart();
//            if (server_type == "UDP") {
//                udp_server.reset();
//            } else if (server_type == "TCP") {
//                tcp_server.reset();
//            }
//
//            returner = server_type + " Server stopped.";
//        } else {
//            returner = server_type + " Server is not running.";
//        }
//    }else if (command == "exit") {
//                if (joker.load()) {
//                    joker.store(false);
//                    io_context.stop();
//                    if (io_thread && io_thread->joinable()) {
//                        io_thread->join();
//                    }
//                    io_context.restart();
//                    udp_server.reset();
//                    tcp_server.reset();
//                }
//                returner = "Exiting...";
//            }
//
//    return returner;
//}

int main() {
    auto screen = ScreenInteractive::Fullscreen();
    //things needed to start and stop the UDPServer
    
    boost::asio::io_context io_context;
    boost::asio::io_service io_service;
    std::unique_ptr<std::thread> io_thread;
    std::unique_ptr<std::thread> udp_thread;
    std::unique_ptr<std::thread> tcp_thread;
    std::unique_ptr<UDPServer> udp_server;
    std::unique_ptr<TCPServer> tcp_server;
    
    SharedVector sharedVec;
    Command cmd(sharedVec);
    
    // Add commands and their responses
    cmd.addCommand("hello", "Hi there!");
    cmd.addCommand("status", "Server is up and running.");
    
    
    
    
    bool running = true;
    // Add commands and their responses
    cmd.addCommand("hello", "Hi there!");
    cmd.addCommand("status", "Server is up and running.");
    
    std::unique_ptr<UDPServer> server;
    
    std::vector<std::string> cli_entries;
    
    //    auto udpStart = [&] {
    //        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
    //        sharedVec.vec.push_back(server_control(io_context, io_thread, udp_server, tcp_server, "start", sharedVec, cmd, "UDP"));
    //    };
    //
    //    auto udpStop = [&] {
    //        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
    //        sharedVec.vec.push_back(server_control(io_context, io_thread, udp_server, tcp_server, "stop", sharedVec, cmd, "UDP"));
    //    };
    //    auto tcpStart = [&] {
    //        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
    //        sharedVec.vec.push_back(server_control(io_context, io_thread, udp_server, tcp_server, "start", sharedVec, cmd, "TCP"));
    //    };
    //
    //    auto tcpStop = [&] {
    //        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
    //        sharedVec.vec.push_back(server_control(io_context, io_thread, udp_server, tcp_server, "stop", sharedVec, cmd, "TCP"));
    //    };
    //
    auto udpStart = [&] {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(udp_server(io_context, udp_thread, server,"start",sharedVec));
    };
    
    auto udpStop = [&] {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(udp_server(io_context, udp_thread, udp_server,"stop",sharedVec));
    };
    auto tcpStart = [&] {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(tcp_control(io_context, tcp_thread, tcp_server,"start",sharedVec,cmd));
    };
    
    auto tcpStop = [&] {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(tcp_control(io_context, tcp_thread, tcp_server,"stop",sharedVec,cmd));
    };
    auto Exit = [&] {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(udp_server(io_context, udp_thread, udp_server,"start",sharedVec));
        running = false;
        screen.Exit();
    };
    
    // -- client list
    // ----------------------------------------------------------------------
    std::array<bool, 10> states;
    
    auto checkboxes = Container::Vertical({});
    for (int i = 0; i < 10; ++i) {
        states[i] = false;
        checkboxes->Add(Checkbox("Client" + std::to_string(i), &states[i]) );
    }
    Component checkframe = Renderer(checkboxes, [&] {
        return checkboxes->Render() | vscroll_indicator | frame |
        size(HEIGHT, LESS_THAN, 5) | border | color(Color::Default);
    });
    checkframe=Wrap("To be Done",checkframe);
    //to be added dynamically later
    
    
    //        // -- Toggle------------------------------------------------------------------
    //        int toggle_selected = 1;
    //        std::vector<std::string> toggle_entries = {
    //            "On",
    //            "Off",
    //        };
    //        auto toggle = Toggle(&toggle_entries, &toggle_selected)| yflex | xflex;
    //        toggle = Wrap("UPD Dis.", toggle);
    //     -- Input ------------------------------------------------------------------
    
    
    std::vector<std::string> input_entries;
    auto input_option = InputOption();
    std::string input_add_content;
    //empty the input after hitting enter.
    input_option.on_enter = [&] {
        input_entries.push_back(input_add_content);
        input_add_content = "";
    };
    Component input_add = Input(&input_add_content, "input files", input_option);
    
    // -- CLI -----------------------------------------------------------------
    
    auto cli_options = InputOption();
    std::string cli_add_content;
    cli_options.on_enter = [&] {
        sharedVec.vec.push_back(cli_add_content);
        cli_add_content = "";
    };
    Component cli_add = Input(&cli_add_content,"commands...",cli_options);
    
    
    auto cliArea = Renderer(cli_add,[&] {
        return window(text("cli input"),vbox(cli_add->Render())) | color(Color::Purple4);
    }) | size(HEIGHT, EQUAL, 1);
    
    // -- UDP Buttons -----------------------------------------------------------------
    
    auto udpbuttons = Container::Horizontal({
        Button("Start", udpStart) | center | color(Color::Green), // turn udp on
        Button("Stop", udpStop) | center | color(Color::Red), // turn udp off
    })| size(HEIGHT, EQUAL, 3);
    udpbuttons = Wrap("UDP Discovery",udpbuttons);
    // -- TCP Buttons -----------------------------------------------------------------
    
    auto tcpbuttons = Container::Horizontal({
        Button("Start", tcpStart) | center | color(Color::Green), // turn udp on
        Button("Stop", tcpStop) | center | color(Color::Red), // turn udp off
        Button("Exit", Exit) | center | color(Color::DarkRed),
    })| size(HEIGHT, EQUAL, 3);
    tcpbuttons = Wrap("TCP Server",tcpbuttons);
    
    // -- Output -----------------------------------------------------------------
    
    auto output = Renderer([&] {
        Elements children = {
            text("Server status, connections and outputs are printed here."),
            separator() | color(Color::Yellow),
        };
        for (size_t i = std::max(0, (int)sharedVec.vec.size() - 12); i < sharedVec.vec.size(); ++i) {
            std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
            children.push_back(text(sharedVec.vec[i]));
        }
        return window(text("Output"), vbox(children) | color(Color::Yellow4) | flex ) | color(Color::Yellow4);
    });
    
    
    // -- Layout -----------------------------------------------------------------
    auto layout = Container::Vertical({
        checkframe,
        input_add,
        udpbuttons,
        tcpbuttons
    }) | size(HEIGHT, GREATER_THAN, 40);
    
    auto component = Renderer(layout, [&] {
        return vbox({
            checkframe->Render(),
            separator() | color(Color::Default),
            //            toggle->Render(),
            //            separator() | color(Color::Default),
            //            input_add->Render(),
            //            separator() | color(Color::Default),
            udpbuttons->Render(),
            separator() | color(Color::Default),
            tcpbuttons->Render()
        }) | size(WIDTH, GREATER_THAN, 30) | borderStyled(ROUNDED,Color::Default);
        
    });
    
    auto topwindows = Container::Horizontal({
        component,
        output | yflex,
    });
    
    auto layoutmain = Container::Vertical({
        topwindows,
        cliArea | flex,
    });
    
    
    auto renderer = Renderer(layoutmain, [&] {
        return layoutmain->Render() ;
    }); //main render for the entire interface.
    
    //create a new thread to run the output continously/force redraw. Required for the output window to update on it's own instead of waiting for an event.
    
    auto screenRedraw = std::thread([&](){
        while(running){
            screen.PostEvent(ftxui::Event::Custom);
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Prevent High CPU Usage.
        }
    });
    
    screen.Loop(renderer);
    
    screenRedraw.join();
    //  screen.Loop(renderer);
}
#include <memory>
#include <atomic>

class TCPServer : public std::enable_shared_from_this<TCPServer> {
public:
    TCPServer(boost::asio::io_service& service, short port, Command& cmd, SharedVector& sharedVec)
        : acceptor_(service, ip::tcp::endpoint(ip::tcp::v4(), port)),
          socket_(service),
          command_(cmd),
          sharedVec_(sharedVec),
          activeSessions_(0) // Initialize activeSessions_ counter
    {
        accept();
    }

    // Decrease activeSessions_ counter when TCPSession is destroyed
    void decreaseActiveSessions() {
        activeSessions_--;
    }

private:
    void accept() {
        auto self = shared_from_this(); // Keep TCPServer instance alive
        acceptor_.async_accept(socket_,
            [this, self](boost::system::error_code ec) {
                if (!ec) {
                    std::cout << "Connection established." << std::endl;
                    auto session = std::make_shared<TCPSession>(std::move(socket_), command_, sharedVec_, self);
                    session->start();
                    // Increase activeSessions_ counter
                    activeSessions_++;
                }
                accept(); // Accept new connections
            });
    }

    ip::tcp::acceptor acceptor_;
    ip::tcp::socket socket_;
    Command& command_;
    SharedVector& sharedVec_;
    std::atomic<int> activeSessions_; // Counter for active sessions
};

class TCPSession : public std::enable_shared_from_this<TCPSession> {
public:
    TCPSession(ip::tcp::socket socket, Command& cmd, SharedVector& sharedVec, std::shared_ptr<TCPServer> server)
        : socket_(std::move(socket)), command_(cmd), sharedVec_(sharedVec), server_(server) {}

    void start() {
        auto self = shared_from_this(); // Keep TCPSession instance alive
        read();
    }

private:
    void read() {
        auto self = shared_from_this(); // Keep TCPSession instance alive
        socket_.async_read_some(buffer(data_),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string received(data_.data(), length);
                    std::cout << "Received: " << received << std::endl;
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

    void write(const std::string& response) {
        auto self = shared_from_this(); // Keep TCPSession instance alive
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

    ip::tcp::socket socket_;
    Command& command_;
    SharedVector& sharedVec_;
    std::shared_ptr<TCPServer> server_; // Pointer to the TCPServer instance
};


#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <mutex>

class Command {};
class SharedVector {};

class TCPSession : public std::enable_shared_from_this<TCPSession> {
public:
    TCPSession(boost::asio::ip::tcp::socket socket, Command& cmd, SharedVector& sharedVec)
        : socket_(std::move(socket)), command_(cmd), sharedVec_(sharedVec) {}

    void start() {
        read();
    }

private:
    void read() {
        socket_.async_read_some(boost::asio::buffer(data_),
            [this](const boost::system::error_code& ec, std::size_t length) {
                if (!ec) {
                    std::string received(data_.data(), length);
                    std::cout << "Received: " << received << std::endl;
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

    void write(const std::string& response) {
        boost::asio::async_write(socket_, boost::asio::buffer(response),
            [this](const boost::system::error_code& ec, std::size_t /*length*/) {
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

    boost::asio::ip::tcp::socket socket_;
    Command& command_;
    SharedVector& sharedVec_;
    std::array<char, 1024> data_; // Buffer for incoming data
};

#include <iostream>
#include <string>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define closesocket close
#endif

#define VER_CMD "++ver\n"
#define MODE_CMD "++mode 1\n"
#define AUTO_CMD "++auto 1\n"
#define IDN_CMD "*idn?\n"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <IP address> <GPIB address>\n";
        return 1;
    }

    const char* ipstr = argv[1];
    const char* gpib = argv[2];

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }
#endif

    int sock;

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        std::cerr << "socket failed\n";
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    struct sockaddr_in service;
    std::memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(ipstr);
    service.sin_port = htons(1234);

    if (connect(sock, reinterpret_cast<struct sockaddr *>(&service), sizeof(service)) < 0) {
        std::cerr << "connect failed\n";
        closesocket(sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    size_t txbytes = strlen(VER_CMD);
    if (send(sock, VER_CMD, static_cast<int>(txbytes), 0) != txbytes) {
        std::cerr << "send failed\n";
        closesocket(sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    char buffer[1024];
    char* ptr = buffer;
    int spaceleft = sizeof(buffer)-1;
    int rxbytes = 0;

    do {
        rxbytes = recv(sock, ptr, spaceleft, 0);
        if (rxbytes < 0) {
            std::cerr << "recv failed\n";
            closesocket(sock);
#ifdef _WIN32
            WSACleanup();
#endif
            return 1;
        }
        ptr += rxbytes;
        spaceleft -= rxbytes;
    } while ((rxbytes != 0) && spaceleft && (*(ptr-1) != '\n'));

    *ptr = 0;
    std::cout << "Version: " << buffer;

    txbytes = strlen(MODE_CMD);
    if (send(sock, MODE_CMD, static_cast<int>(txbytes), 0) != txbytes) {
        std::cerr << "send failed\n";
        closesocket(sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    txbytes = strlen(AUTO_CMD);
    if (send(sock, AUTO_CMD, static_cast<int>(txbytes), 0) != txbytes) {
        std::cerr << "send failed\n";
        closesocket(sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    txbytes = snprintf(buffer, sizeof(buffer), "++addr %s\n", gpib);
    if (send(sock, buffer, static_cast<int>(txbytes), 0) != txbytes) {
        std::cerr << "send failed\n";
        closesocket(sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    txbytes = strlen(IDN_CMD);
    if (send(sock, IDN_CMD, static_cast<int>(txbytes), 0) != txbytes) {
        std::cerr << "send failed\n";
        closesocket(sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    ptr = buffer;
    spaceleft = sizeof(buffer)-1;

    do {
        rxbytes = recv(sock, ptr, spaceleft, 0);
        if (rxbytes < 0) {
            std::cerr << "recv failed\n";
            closesocket(sock);
#ifdef _WIN32
            WSACleanup();
#endif
            return 1;
        }
        ptr += rxbytes;
        spaceleft -= rxbytes;
    } while ((rxbytes != 0) && spaceleft && (*(ptr-1) != '\n'));

    *ptr = 0;
    std::cout << "*IDN? response: " << buffer;

    closesocket(sock);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
