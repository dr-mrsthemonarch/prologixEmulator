cmake_minimum_required(VERSION 3.26)

project(prologix_emulator LANGUAGES CXX VERSION 1.0.0)

# Boost
set(BOOST_INCLUDE_LIBRARIES thread filesystem system program_options asio date_time)
set(BOOST_ENABLE_CMAKE ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
include(FetchContent)

FetchContent_Declare(
        Boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_PROGRESS TRUE
        GIT_TAG boost-1.85.0 # Update to the required Boost version
)
FetchContent_MakeAvailable(Boost)

# FTXUI
FetchContent_Declare(
        ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG main # Important: Specify a version or a commit hash here.
)
FetchContent_MakeAvailable(ftxui)

# Include Boost and FTXUI headers
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${ftxui_SOURCE_DIR}/include)

# Add source files to the project
set(SOURCES
        src/Commander.cpp
        src/Functions.cpp
        src/Parser.cpp
        src/Responder.cpp
        src/TCPServer.cpp
        src/TCPSession.cpp
        src/UDPServer.cpp
        src/main.cpp
)

set(HEADERS
        src/Commander.h
        src/Functions.h
        src/Parser.h
        src/Responder.h
        src/TCPServer.h
        src/TCPSession.h
        src/UDPServer.h
)

# Add executable
add_executable(prologix_emulator ${SOURCES} ${HEADERS})

# Link FTXUI libraries
target_link_libraries(prologix_emulator
        PRIVATE ftxui::component
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom

)

# Link Boost libraries
target_link_libraries(prologix_emulator
        PRIVATE Boost::asio
        PRIVATE Boost::filesystem
        PRIVATE Boost::thread
        PRIVATE Boost::program_options
        PRIVATE Boost::system
)

# Set C++ standard
if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET prologix_emulator PROPERTY CXX_STANDARD 20)
endif()

# Ensure Boost is properly found
if (NOT TARGET Boost::asio OR NOT TARGET Boost::filesystem OR NOT TARGET Boost::thread OR NOT TARGET Boost::program_options)
    message(FATAL_ERROR "Required Boost libraries not found!")
endif()

# Ensure FTXUI is properly found
if (NOT TARGET ftxui::screen OR NOT TARGET ftxui::dom)
    message(FATAL_ERROR "FTXUI libraries not found!")
endif()
