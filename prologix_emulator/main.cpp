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

// Function to control the UDPserver
std::string udp_server(boost::asio::io_context& io_context,
                       std::unique_ptr<std::thread>& io_thread,
                       std::unique_ptr<UDPServer>& server,
                       std::string command,
                       SharedVector& sharedVec) {
    std::string returner;
    
    if (command == "start") {
        if (!batman.load()) {
            batman.store(true);
            // Create a new server instance and run it in a separate thread
            server = std::make_unique<UDPServer>(io_context, 3040,sharedVec);
            
            io_thread = std::make_unique<std::thread>([&io_context]() {
                io_context.run();
            });
            returner = "Starting UDP Discovery...";
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
    }else if (command == "exit") {
                if (batman.load()) {
                    batman.store(false);
                    io_context.stop();
                    if (io_thread && io_thread->joinable()) {
                        io_thread->join();
                    }
                    io_context.restart();
                    server.reset();
                }
                returner = "Exiting...";
            }
    return returner;
}

// Function to control the TCPserver
std::string tcp_control(boost::asio::io_context& io_context,
                        std::unique_ptr<std::thread>& io_thread,
                        std::unique_ptr<TCPServer>& server,
                        const std::string& command,
                        SharedVector& sharedVec,
                        Command& cmd) {
    std::string returner;
    
    if (command == "start") {
        if (!joker.load()) {
            joker.store(true);
            // Create a new TCP server instance and run it in a separate thread
            server = std::make_unique<TCPServer>(io_context, 1234, cmd, sharedVec);
            
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
            server.reset();
            
            returner = "TCP Server stopped.";
        } else {
            returner = "TCP Server is not running.";
        }
    }else if (command == "exit") {
                if (joker.load()) {
                    joker.store(false);
                    io_context.stop();
                    if (io_thread && io_thread->joinable()) {
                        io_thread->join();
                    }
                    io_context.restart();
                    server.reset();
                }
                returner = "Exiting...";
            }
    
    return returner;
}

int main() {
    auto screen = ScreenInteractive::Fullscreen();
    //things needed to start and stop the UDPServer
    SharedVector sharedVec;
    boost::asio::io_context udpcontext;
    boost::asio::io_context tcpcontext;
    boost::asio::io_service io_service;
    bool running = true;
    
    std::unique_ptr<UDPServer> udpserver;
    std::unique_ptr<TCPServer> tcpserver;
    std::unique_ptr<std::thread> udpthread;
    std::unique_ptr<std::thread> tcpthread;
    std::vector<std::string> cli_entries;
    Command cmd(sharedVec);
    
    // Add commands and their responses
    cmd.addCommand("hello", "Hi there!");
    cmd.addCommand("status", "Server is up and running.");
    cmd.addCommand("++ver", "Prologix v0.1\n");
    cmd.addCommand("++mode", "1\n");
    cmd.addCommand("++auto", "1\n");
    cmd.addCommand("*idn?", "I am a emulator\n");
    
    // ----------------------------------------------------------------------
    auto udpStart = [&] {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(udp_server(udpcontext, udpthread, udpserver,"start",sharedVec));
    };
    
    auto udpStop = [&] {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(udp_server(udpcontext, udpthread, udpserver,"stop",sharedVec));
    };
    auto tcpStart = [&] {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(tcp_control(tcpcontext, tcpthread, tcpserver,"start",sharedVec,cmd));
    };
    
    auto tcpStop = [&] {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(tcp_control(tcpcontext, tcpthread, tcpserver,"stop",sharedVec,cmd));
    };
    auto Exit = [&] {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(udp_server(udpcontext, udpthread, udpserver,"exit",sharedVec));
        sharedVec.vec.push_back(tcp_control(tcpcontext, tcpthread, tcpserver,"exit",sharedVec,cmd));
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
    }
                            ) | size(HEIGHT, EQUAL, 1);
    
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
        //        toggle,
        input_add,
        udpbuttons,
        tcpbuttons
    }) | size(HEIGHT, GREATER_THAN, 40);
    
    auto component = Renderer(layout, [&] {
        return vbox({
            checkframe->Render(),
            separator() | color(Color::Default),
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
}

