
#include "Acceptor.h"
#include "InetAddress.h"
#include "TcpStream.h"

#include <string.h>
#include <sys/time.h>
#include <thread>

long long g_bytes;

double now()
{
  struct timeval tv = { 0, 0 };
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

std::string getMessage()
{
  std::string line;
  for (int i = 33; i < 127; ++i)
  {
    line.push_back(char(i));
  }
  line += line;

  std::string message;
  for (size_t i = 0; i < 127-33; ++i)
  {
    message += line.substr(i, 72) + '\n';
  }
  return message;
}

void measure()
{
  double start = now();
  while (true)
  {  
  	__sync_lock_test_and_set(&g_bytes, 0);
    struct timespec ts = { 1, 0 };
    ::nanosleep(&ts, NULL);
    // unfortunately, those two assignments are not atomic
  
    double end = now();
    double elapsed = end -  start;
    start = end;
    if (g_bytes)
    {
      printf("%.3f MiB/s\n", g_bytes / (1024.0 * 1024) / elapsed);
    }
  }
}

void chargen(TcpStreamPtr stream)
{
  std::string message = getMessage();
  while (true)
  {
    int nw = stream->sendAll(message.data(), message.size());
    __sync_fetch_and_add(&g_bytes, nw);
    if (nw < static_cast<int>(message.size()))
    {
      break;
    }
  }
}

// a thread-per-connection current chargen server and client
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

      std::thread thr(chargen, std::move(tcpStream));
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
        chargen(std::move(stream));
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
