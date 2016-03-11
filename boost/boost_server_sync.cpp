#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
	
	
int main()
{
	io_service io_service;
	ip::tcp::acceptor acceptor(io_service,ip::tcp::endpoint(ip::tcp::v4(),10000));
	while(1)
	{
		ip::tcp::socket socket(io_service);
		acceptor.accept(socket);
		std::cout<<socket.remote_endpoint().address()<<std::endl;
				
		boost::system::error_code ec;
		char buf[1024] = { 0};
		socket.read_some(buffer(buf),ec);
		socket.write_some(buffer(buf),ec);
		if (ec)
			{
				std::cout<<boost::system::system_error(ec).what()<<std::endl;
					break;
			}	
	}
	return 0;
};