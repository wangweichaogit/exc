/*
检测接受数据的平均速度
chargen  作为message发送方，全力发送
discard  作为message接收方，记录接收到的字节数
全局g_bytes 使用__sync* 实现无锁操作
*/

#include "Acceptor.h"
#include "InetAddress.h"
#include "TcpStream.h"

#include <string.h>
#include <thread>
#include <sys/time.h>
long long g_bytes ;

double now()
{
  struct timeval tv = { 0, 0 };
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}
void measure()
{
  double start = now();
  while (true)
  {	
  	__sync_lock_test_and_set(&g_bytes, 0);
		struct timespec ts = { 1, 0 };
    ::nanosleep(&ts, NULL);
    
  	double end = now();
    double elapsed = end - start;
    start = end;
    if (g_bytes)
    {
      printf("%.3f MiB/s\n", g_bytes / (1024.0 * 1024) / elapsed);
    }
  }
}

void discard(TcpStreamPtr stream)
{
  char buf[65536];
  while (true)
  {
    int nr = stream->receiveSome(buf, sizeof buf);
    if (nr <= 0)
      break;
      __sync_fetch_and_add(&g_bytes, nr);
  }
}

// a thread-per-connection current discard server and client
int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    printf("Usage:\n  %s hostname port\n  %s -l port\n", argv[0], argv[0]);
    return 0;
  }

  std::thread(measure).detach();

  int port = atoi(argv[2]);
  if (strcmp(argv[1], "-l") == 0)
  {
    Acceptor acceptor((InetAddress(port)));
    printf("Accepting... Ctrl-C to exit\n");
    int count = 0;
    while (true)
    {
      TcpStreamPtr tcpStream = acceptor.accept();
      printf("accepted no. %d client\n", ++count);

      std::thread thr(discard, std::move(tcpStream));
      thr.detach();
    }
  }
  else
  {
    InetAddress addr(port);
    const char* hostname = argv[1];
    if (InetAddress::resolve(hostname, &addr))
    {
      TcpStreamPtr stream(TcpStream::connect(addr));
      if (stream)
      {
        discard(std::move(stream));
      }
      else
      {
        printf("Unable to connect %s\n", addr.toIpPort().c_str());
        perror("");
      }
    }
    else
    {
      printf("Unable to resolve %s\n", hostname);
    }
  }
}
