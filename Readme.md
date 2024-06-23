# prologixEmulator
Turn any computer into a Prologix Ethernet Emulator.
![alt text](https://github.com/dr-mrsthemonarch/prologixEmulator/blob/main/images/preview.png?raw=true)

## Current Version 0.3

## What is this?
This is my retro style prologix emulator.

What is a prologix, and why does it need an emulator?  Basically it is a hardware controller made up by some amazing people to interface with an ancient interface/protocol developed in the neolithic age, called **GPIB**.

This interface is something like i2c. It was used extensively in the past with a lot of expensive scientific devices to talk to computers easily in some unified standard, oddly enough even in 2024 *many* of these devices still exist are still in use and still use this ancient protocol instead of using more modern field bus' like modbus, ethercat, blah blah.

The device itself plugs into ethernet and a gbip device and does all the dirty work translating strings into gpib gibberis. I've used them for stanford power supplies but don't have regular access to them to do testing with over networks. Often time the internet infrastructures in labs are behind many routers and switches. The prologix controllers can only be found with prologix' netfinder utility when they are behind a switch using udp discovery, which isn't always obvious.


Hence this project was born, reverse engineering a prologix controller, I, or anyone can simply run the emulator, and get to work ensuring prologix devices are discoverable over a network, develop user interfaces of some kind to talk to your devices, or well whatever you'd do with a highly dedicated ethernet controller for a billion year old communication protocol to talk to your weird science device from the 50s.

## What features are there?

It's currently a bit bare bones, all i've done so far are:
- Providing a basic interface including buttons and output window, totally retro!
- A UDP Discovery Server. This will allow you to find the server like any prologix device over a network. If Netfinder can find it, or gpib configuration utility, then it'll find the real thing too!
- A TCP Server, The basic internet connection. Multiple clients are supported through multithreading, so you only need one prologix emulator running and you can connect with as many clients as you want!

## To Do/Bugfixing:
- ~~Dynamically display connected clients~~
  - Fix interactivity of clients
- Add localhost client to cli input for testing the server and commands and expected responses within the server itself.
- change static and dynamic ips via Netfinder
- Fix the Discovery packet structure to properly display uptime, actual mac address and some other things.
- There is an odd segfault when clicking udp or tcp start and stop buttons too often.
- Add a "devices" dropdown menu for various emulated GPIB devices.
    - Stanford PSU 350/1

### Firmware to implement
- Remaining commands:
    - clr
    - read
    - rst
    - savecfg
    - spoll
    - status
    - trg
    - help




## Build and Requirements
To build the program yourself, you can use cmake and the provided CMakelist.txt to compile the program, it will automatically fetch all the required libraries to build yourself. I haven't tried on any other OS besides macOS, but in theory everything will work fine.

If you have a nonsense IDE like Xcode, which I used to use until some days ago as of this writing you will have to prebuild and link the following libraries:

- Boost with asio
- ftxui

Otherwise, using cmake is really the way to go, since you don't need anything except commandline...
Download the source, and compile using the following commands, assuming you're going the cmake route.

**_This currently only works on macOS on arm64, I have yet to compile successfully on windows!_**

```
mkdir build && cd build
cmake ..
make -j8 (or less depending on how many cores you have)
cd ../binary
./prologix_emulator

```
If you just compile the executable to use and never want to recompile again, you can delete everything in the source afterwards

## FAQ
### Does the cli Interface do anything?
  - Nope! Not yet, the idea is to send commands locally to the server itself. This was mostly a debugging feature in the past, and will likely disappear in the future.
### Cmake fails when trying to fetch
  - This I've had happen several times due to the lack of connecting to github, it may be your work network blocking that traffic, or something weird with the internet, but has nothing to do with the project.