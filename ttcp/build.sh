#! /bin/bash

SRC="InetAddress.cc Socket.cc TcpStream.cc Acceptor.cc"

g++ -std=c++11 -Wall -Wextra -g -O2 $SRC ttcp.cc -o ttcp -lboost_program_options
