SRC=Socket.cc InetAddress.cc TcpStream.cc Acceptor.cc

all:
	g++ -std=c++11 -Wall $(SRC) nodelayclient.cc -o nodelayclient
	g++ -std=c++11 -Wall $(SRC) nodelayserver.cc -o nodelayserver

clean:
	-rm -rf nodelayclient nodealyserver

.PHONY:clean
