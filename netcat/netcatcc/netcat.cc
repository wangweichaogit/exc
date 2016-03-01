#include "InetAddress.h"
#include "TcpStream.h"
#include "Acceptor.h"
#include <thread>
#include <string.h>
#include <unistd.h>

//  发送所有数据
int write_n(int fd,const char * buf,int length)
{
	int written = 0;
	while (written < length) {
		int nw = ::write(fd,static_cast<const char *>(buf) + written , length -written);
		if (nw > 0)
			written += nw;
		else if (nw == 0)
			break;
		else if (errno != EINTR) {
			perror("write");
			break;
		}
	}
	return written;
}


void run(TcpStreamPtr stream)
{
	std::thread thr([&stream] () {
		char buf[8192];	
		int nr = 0;
		while ( (nr = stream->receiveSome(buf,sizeof(buf))) > 0) {
			int nw = write_n(STDOUT_FILENO,buf,nr);
			if (nw < nr) break;
		}
	
	::exit(0);
	
	});
	
	char buf[8192];
	int nr = 0;
	while ((nr = ::read(STDIN_FILENO,buf,sizeof(buf))) > 0) {
		int nw = stream->sendAll(buf,nr);
		if (nw < nr) break;
	}
	stream->shutdownWrite();
	thr.join();
}


int main(int argc,const char * argv[])
{
	if (argc < 3)
	{
		printf("usage: \n %s hostname port\n %s-l port\n",argv[0],argv[0]);
		return 0;
	}
	
	int port = atoi(argv[2]);
	if (strcmp(argv[1],"-l") == 0)
	{
		std::unique_ptr<Acceptor> acceptor(new Acceptor(InetAddress(port)));
		TcpStreamPtr stream(acceptor->accept());
		if (stream)
		{
			acceptor.reset();
			run(std::move(stream));
		}
		else
			perror("accept");
		
	}
	else {
		InetAddress addr(port);
		const char * hostname = argv[1];
		if (InetAddress::resolve(hostname,&addr))
		{
			TcpStreamPtr stream(TcpStream::connect(addr));
			if (stream)
				run(std::move(stream));
			else	{
				printf("unable to connect %s\n",addr.toIpPort().c_str());
			}
		}
		else
			printf("unable to resolve %s\n",hostname);
	
	}
}
