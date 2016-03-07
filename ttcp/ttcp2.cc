#include "Acceptor.h"
#include "InetAddress.h"
#include "TcpStream.h"

#include <iostream>
#include <boost/program_options.hpp>
#include <sys/time.h>


struct Options
{
	uint16_t port;
	int 			length;
	int 			number;
	
};