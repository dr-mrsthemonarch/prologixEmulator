# prologixEmulator
Turn any computer into a Prologix Ethernet Emulator.
![alt text](https://github.com/dr-mrsthemonarch/prologixEmulator/blob/main/images/preview.png?raw=true)

## Current Version 0.1

## What is this?
This is my retro style prologix emulator.

What is a prologix, and why does it need an emulator? Well that's a story and a half. But basically it is a hardware controller made up by some amazing people to interface with an ancient interface/protocol developed in the neolithic age, called **GPIB**.

This interface is something like i2c. It was used extenstively in the past with a lot of scientific devices to talk to computers easily in some unified standard, oddly enough even in 2024 and beyond *many* of these devices still exist, and still use this ancient protocol instead of using more modern fieldbus' like modbus, ethercat, blah blah. 

The device itself plugs into ethernet, and a gbip device and does all the dirty work translating strings into gbip gibberish, I use them for stanford power supplies, but don't have regular access to them to do testing with over networks, I also don't want to carry around a whole PSU just to see if the thing works on a network. 

Hence this project was born, reverse engineering a prologix controller, I, or anyone can simply run the emulator, and get to work ensuring prologix devices are discoverable over a network, develop a user interface of some kind to talk to your devices, or well whatever you'd do with a highly dedicated ethernet controller for a billion year old communication protocol to talk to your weird science device from the 50s.

## What features are there?

It's currently a bit bare bones, all i've done so far are:
- Providing a basic interface including buttons and output window, full retro!
- A UDP Discovery Server. This will allow you to find the server like any prologix device over a network. If Netfinder can find it, or gbip configuration utility, then it'll find the real thing too!
- A TCP Server, The basic internet connection. Multiple clients are supported through multithreading, so you only need one prologix emulator running. 
- A Commander, This is just a simple class so far and more or less completely under the hood, but eventually it'll be all implemented firmware functions, just like the real thing.

### To Do/Bugfixing:
- Dynamically display connected clients
- Add localhost client to cli input for testing the server and commands and expected responses within the server itself.
- change static and dynamic ips via Netfinder
- Fix the Discovery packet structure to properly display uptime, actual mac address and some other things.
- Add a "devices" dropdown menu for various emulated GPIB devices.
    - Stanford PSU 350/1

#### Firmware to implement
 - Most ++commands
    - clr
    - read
    - rst
    - savecfg
    - spoll
    - status
    - trg    
    - help
    - ~~addr~~ 
    - ~~auto~~
    - ~~eoi~~
    - ~~eos~~
    - ~~eot_enable~~
    - ~~eot_char~~
    - ~~ifc~~
    - ~~llo~~
    - ~~loc~~
    - ~~lon~~
    - ~~mode~~
    - ~~read_tmo_ms~~
    - ~~srq~~
    - ~~ver~~

 


### Build and Requirements
To build the program yourself, you can use cmake and the provided CMakelist.txt to compile the program, it will automatically fetch all of the required libraries to build yourself. I haven't tried on any other OS besides macOS, but in theory everything will work fine.

- std=c++20 compatible compiler, like modern clang or gcc
- Boost with asio
- ftxui

Download the source code, 

```
mkdir build && cd build
cmake ..
make -j8 (or less depending on how many cores you have)
./prologix_emulator

```

I would suggest deleting the boost source file, since it's around 2gb, but I don't know another way to automagically fetch and build boost and keep the file sizes small.
