# Overall Design #
This is being designed as a client server program. We have a client side which will run on Windows, Mac, and Linux. We have a server program that will run on Linux, Mac, and Windows.

The client side, server side, and network library are all written in C++.

## Client Side ##
The client side consists of a gui and the networking library. The gui is written using FLTK.

## Server Side ##
The server side consists of a database, the core server program and the networking library. The database we are using is postgres.

## Networking Library ##
The networking library is written using ENet, a networking layer that implements a pseudo TCP stack using UDP. There are data structures that are written on top of ENet that make communication between the client and server possible.