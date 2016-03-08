SRC=InetAddress.cc Acceptor.cc TcpStream.cc Socket.cc

all:
	g++ -std=c++11 echoserver.cc $(SRC) -o echoserver -Wall  -lpthread
	g++ -std=c++11 echoclient.cc $(SRC) -o echoclient -Wall  -lpthread

clean:
	rm -rf echoclient echoserver

.PHONY:clean
