#ifdef _WIN64
#include <SDKDDKVer.h>
#define _WIN32_WINDOWS _WIN32_WINNT_WIN10
#elif _WIN32
#error "Build only supports x86-64 and does not support x86"
#endif // _WIN64
#include <boost/asio.hpp>
#include <vector>
#include <string>
#include <array>
#include "UDPServer.h"
#include "Functions.h"
#include "TCPServer.h"
#include "Commander.h"
#include "ftxui/component/component.hpp"       // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, operator|, text, center, border

using boost::asio::ip::udp;
using namespace ftxui;
std::atomic<bool> batman(false); // Use atomic for thread-safe boolean
std::atomic<bool> joker(false); // Use atomic for thread-safe boolean

// Function to control the UDPserver
std::string udp_server(boost::asio::io_context &io_context,
                       std::unique_ptr<std::thread> &io_thread,
                       std::unique_ptr<UDPServer> &server,
                       const std::string &command,
                       SharedVector &sharedVec) {
    std::string returner;

    if (command == "start") {
        if (!batman.load()) {
            batman.store(true);
            // Create a new server instance and run it in a separate thread
            server = std::make_unique<UDPServer>(io_context, 3040, sharedVec);

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
    } else if (command == "exit") {
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
std::string tcp_control(boost::asio::io_context &io_context,
                        std::unique_ptr<std::thread> &io_thread,
                        std::unique_ptr<TCPServer> &server,
                        const std::string &command, Commander &cmd,
                        SharedVector &sharedVec,
                        SharedVector &clientVec) {
    std::string returner;

    if (command == "start") {
        if (!joker.load()) {
            joker.store(true);
            // Create a new TCP server instance and run it in a separate thread
            server = std::make_unique<TCPServer>(io_context, 1234, cmd, sharedVec, clientVec);

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
    } else if (command == "exit") {
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
    SharedVector clientVec;
    initializeSharedVector(clientVec, {}); // start the vector empty for the program doesn't crash
    boost::asio::io_context udpcontext;
    boost::asio::io_context tcpcontext;
    boost::asio::io_service io_service;
    bool running = true;

    std::unique_ptr<UDPServer> udpserver;
    std::unique_ptr<TCPServer> tcpserver;
    std::unique_ptr<std::thread> udpthread;
    std::unique_ptr<std::thread> tcpthread;
    std::vector<std::string> cli_entries;
    Commander commander(sharedVec);

    // Add firmware commands and their responses
    commander.addCommand("++addr", "14", 2);
    commander.addCommand("++reset", "", 0);
    commander.addCommand("++ver", "1.3.4", 0);
    commander.addCommand("++mode", "0", 1);
    commander.addCommand("++auto", "0", 1);
    commander.addCommand("++eoi", "0", 1);
    commander.addCommand("++eos", "0", 1);
    commander.addCommand("++eot_enable", "0", 1);
    commander.addCommand("++eot_char", "0", 1);
    commander.addCommand("++ifc", "Conntroller in Charge", 0);
    commander.addCommand("++llo", "Display Off", 0);
    commander.addCommand("++loc", "Display On", 0);
    commander.addCommand("++lon", "0", 1);
    commander.addCommand("++read_tmo_ms", "1", 1);
    commander.addCommand("++srq", "0", 1);
    commander.addCommand("++status", "0", 1);
    //Add PSU350 Commands
    commander.addCommand("HVON", "0", 1);
    commander.addCommand("HVOF", "0", 1);
    commander.addCommand("VOUT?", "0", 1);
    commander.addCommand("VSET", "0", 1);

    // ----------------------------------------------------------------------
    auto udpStart = [&] {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(udp_server(udpcontext, udpthread, udpserver, "start", sharedVec));
    };

    auto udpStop = [&] {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(udp_server(udpcontext, udpthread, udpserver, "stop", sharedVec));
    };
    auto tcpStart = [&] {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(tcp_control(tcpcontext, tcpthread, tcpserver, "start", commander, sharedVec,
                                            clientVec));
    };

    auto tcpStop = [&] {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(tcp_control(tcpcontext, tcpthread, tcpserver, "stop", commander, sharedVec, clientVec));
    };
    auto Exit = [&] {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        sharedVec.vec.push_back(udp_server(udpcontext, udpthread, udpserver, "exit", sharedVec));
        sharedVec.vec.push_back(tcp_control(tcpcontext, tcpthread, tcpserver, "exit", commander, sharedVec, clientVec));
        running = false;
        screen.Exit();
    };

    // -- client list
    // ----------------------------------------------------------------------
    std::array<bool, 10> states;

    auto lister = Renderer([&] {
        Component clist = Container::Vertical({});
    for (size_t i = std::max(0, (int) clientVec.vec.size() - 9); i < clientVec.vec.size(); ++i) {
            std::lock_guard<std::mutex> lock(clientVec.vecMutex);
            clist ->Add(Checkbox(clientVec.vec[i], &states[i]));
        };
        return clist->Render() | size(HEIGHT, EQUAL, 3) | vscroll_indicator ;
    });

    lister = Wrap("Clients", lister);

    // -- CLI -----------------------------------------------------------------

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
    Component cli_add = Input(&cli_add_content, "commands...", cli_options);


    auto cliArea = Renderer(cli_add, [&] {
        return window(text("cli input"), vbox(cli_add->Render())) | color(Color::Purple4);
    }) | size(HEIGHT, EQUAL, 1);

    // -- UDP Buttons -----------------------------------------------------------------

    auto udpbuttons = Container::Horizontal({
                          Button("Start", udpStart) | center | color(Color::Green), // turn udp on
                          Button("Stop", udpStop) | center | color(Color::Red), // turn udp off
                      }) | size(HEIGHT, EQUAL, 3);
    udpbuttons = Wrap("UDP Discovery", udpbuttons);
    // -- TCP Buttons -----------------------------------------------------------------

    auto tcpbuttons = Container::Horizontal({
                          Button("Start", tcpStart) | center | color(Color::Green), // turn udp on
                          Button("Stop", tcpStop) | center | color(Color::Red), // turn udp off
                          Button("Exit", Exit) | center | color(Color::DarkRed),
                      }) | size(HEIGHT, EQUAL, 5);
    tcpbuttons = Wrap("TCP Server", tcpbuttons);

    // -- Output -----------------------------------------------------------------

    auto output = Renderer([&] {
        Elements children = {
            text("Server status, connections and outputs are printed here."),
            separator() | color(Color::Yellow),
        };
        for (size_t i = std::max(0, (int) sharedVec.vec.size() - 9); i < sharedVec.vec.size(); ++i) {
            std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
            children.push_back(text(sharedVec.vec[i]));
        }
        return window(text("Output"), vbox(children) | color(Color::Yellow4) | xflex | yflex) | color(Color::Yellow4);
    });


    // -- Layout -----------------------------------------------------------------
    auto layout = Container::Vertical({
        lister,
        input_add,
        udpbuttons,
        tcpbuttons
    });

    auto component = Renderer(layout, [&] {
        return vbox({
                   lister->Render() | vscroll_indicator,
                   separator() | color(Color::Default),
                   udpbuttons->Render(),
                   separator() | color(Color::Default),
                   tcpbuttons->Render()
               }) | size(WIDTH, GREATER_THAN, 30) | size(HEIGHT, EQUAL, 10) | borderStyled(ROUNDED, Color::Default);
    });

    auto topwindows = Container::Horizontal({
        component,
        output | size(HEIGHT, EQUAL, 13) | yflex,
    });

    auto layoutmain = Container::Vertical({
        topwindows,
        cliArea | flex,
    });


    auto renderer = Renderer(layoutmain, [&] {
        return layoutmain->Render();
    }); //main render for the entire interface.

    //create a new thread to run the output continously/force redraw. Required for the output window to update on it's own instead of waiting for an event.

    auto screenRedraw = std::thread([&]() {
        while (running) {
            screen.PostEvent(ftxui::Event::Custom);
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Prevent High CPU Usage.
        }
    });

    screen.Loop(renderer);

    screenRedraw.join();
}
