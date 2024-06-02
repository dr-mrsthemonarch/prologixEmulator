# prologixEmulator
Turn your computer into a Prologix Ethernet Emulator.
![alt text](https://github.com/dr-mrsthemonarch/prologixEmulator/blob/main/images/preview.png?raw=true)

## Current Version 0.1

## What is this?
This is my retro style prologix emulator.

What is a prologix, and why does it need an emulator? Well that's a story and a half. But basically it is a hardware controller made up by some amazing people to interface with an ancient interface/protocol developed in the neolithic age, called **GBIP**.

This interface is something like i2c. It was used extenstively in the past with a lot of scientific devices to talk to computers easily in some unified standard, oddly enough even in 2024 and beyond *many* of these devices still exist, and still use this ancient protocol instead of using more modern fieldbus' like modbus, ethercat, blah blah. 

The device itself plugs into ethernet, and a gbip device and does all the dirty work translating strings into gbip gibberish, I use them for stanford power supplies, but don't have regular access to them to do testing with over networks, I also don't want to carry around a whole PSU just to see if the thing works on a network. 

Hence this project was born, reverse engineering a prologix controller, I, or anyone can simply run the emulator, and get to work ensuring prologix devices are discoverable over a network, develop a user interface of some kind to talk to your devices, or well whatever you'd do with a highly dedicated ethernet controller for a billion year old communication protocol to talk to your weird science device from the 50s.

## What features are there?

It's currently a bit bare bones, all i've done so far are:
- Providing a basic interface including buttons and output window, full retro!
- A UDP Discovery Server. This will allow you to find the server like any prologix device over a network. If Netfinder can find it, or gbip config, then it'll find the real thing too!
- A TCP Server, This is the main component of the whole thing. It accepts connections and then displays any commands received, also outputting responses.
- A Commander, This is just a simple class so far and more or less completely under the hood, but eventually it'll be all implemented firmware functions, just like the real thing.

### To Do:
- Lots of stuff, update later.

### Requirements
The compiled program needs nothing but to just be run, if you plan to compile it from scratch, you'll need:
- Boost with asio
- ftxui
