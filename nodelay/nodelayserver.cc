/*
测试nodelay选项对程序延时的影响
*/

#include "InetAddress.h"
#include "Acceptor.h"
#include "TcpStream.h"

#include <assert.h>
#include <string.h>
#include <sys/time.h>



int main(int argc,char *argv[])
{
	InetAddress listenaddress(32100);
	Acceptor acceptor(listenaddress);
	printf("Listening Port 3210\n");
	printf("Accepting... Ctrl-C to exit\n");
	int count = 0;
	while (true) {
		TcpStreamPtr stream(acceptor.accept());
		printf("accepted no. %d client\n", ++count);
		
		
		while (true)
    {
      int len = 0;
      int nr = stream->receiveAll(&len, sizeof len);
      if (nr <= 0)
        break;
      printf("received header %d bytes, len = %d\n", nr, len);
      assert(nr == sizeof len);

      std::vector<char> payload(len);
      nr = stream->receiveAll(payload.data(), len);
      printf("received payload %d bytes\n", nr);
      assert(nr == len);
      int nw = stream->sendAll(&len, sizeof len);
      assert(nw == sizeof len);
    }
    printf("no. %d client ended.\n", count);
	}
}