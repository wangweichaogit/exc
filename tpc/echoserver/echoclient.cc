#include "InetAddress.h"
#include "TcpStream.h"
#include <unistd.h>

int main(int argc,char *argv[])
{
	if (argc < 3)
	{
		printf("usage:%s hostname port message_length\n",argv[0]);
		return 0;
	}
	
	const int len = atoi(argv[3]);
	InetAddress addr(atoi(argv[2]));
	if (!InetAddress::resolve(argv[1], &addr))
  {
    printf("Unable to resolve %s\n", argv[1]);
    return 0;
  }
  
  printf("connecting to %s\n", addr.toIpPort().c_str());
  TcpStreamPtr stream(TcpStream::connect(addr));
 	if (!stream)
  {
    perror("");
    printf("Unable to connect %s\n", addr.toIpPort().c_str());
    return 0;
  }
  printf("connected, sending %d bytes\n", len);

  std::string message(len, 'S');
  int nw = stream->sendAll(message.c_str(), message.size());
  printf("sent %d bytes\n", nw);
  
  std::vector<char> receive(len);
  int nr = stream->receiveAll(receive.data(), receive.size());
  printf("received %d bytes\n", nr);
  if (nr != nw)
  {
    printf("!!! Incomplete response !!!\n");
  }
}