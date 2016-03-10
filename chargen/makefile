SRC=InetAddress.cc Acceptor.cc TcpStream.cc Socket.cc

all:
	g++ -std=c++11 discard.cc $(SRC) -o discard -Wall  -lpthread
	g++ -std=c++11 chargen.cc $(SRC) -o chargen -Wall  -lpthread

clean:
	rm -rf  discard chargen

.PHONY:clean
