#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <string>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
using namespace std;
using namespace boost::asio;

class acceptimpl
{
	public:
		ip::tcp::socket socket;
		acceptimpl(io_service& io_service):socket(io_service)
			{
			}
};
void function(void *arg)
{
	acceptimpl *impl = (acceptimpl *)arg;
	char buf[1024] = { 0 };
	impl->socket.read_some(buffer(buf));
	impl->socket.write_some(buffer(buf));
	impl->socket.close();
	cout<<"done"<<endl;
	delete impl;
}
bool NewThread(void(*pfn)(void*), void* parg)
{
    try
    {
        boost::thread(pfn, parg); // thread detaches when out of scope
    } catch(boost::thread_resource_error &e) {
        printf("Error creating thread: %s\n", e.what());
        return false;
    }
    return true;
}

void handler(	 const boost::system::error_code& error, 
  	 std::size_t bytes_transferred )
{
	
}

void handle_accept(boost::shared_ptr<ip::tcp::acceptor> acceptor,acceptimpl *impl)
{
	cout<< impl->socket.remote_endpoint().address()<<":"<<endl;
	
	//新建一个异步连接
		
	acceptimpl  *impl2 = new acceptimpl(acceptor->get_io_service());
	acceptor->async_accept(impl2->socket,boost::bind(&handle_accept,acceptor,impl2));
	
	NewThread(function,(void *)impl);
}
int main()
{
	io_service io_service;
	ip::address bindAddress = ip::address_v4::loopback();
	//绑定loopback（）  和 any（）的区别
	//第一个进程将绑定ip+port是 127.0.0.1：port；意味着服务只能接受来自本地的connect
	//第一个进程将绑定ip+port是 ：：：port；  是个网络程序
	ip::tcp::endpoint endpoint(bindAddress,8888);
	boost::signals2::signal<void ()> StopRequests;
	boost::shared_ptr<ip::tcp::acceptor> acceptor(new ip::tcp::acceptor(io_service));
	acceptimpl  *impl = new acceptimpl(acceptor->get_io_service());
	try{
			acceptor->open(endpoint.protocol());
			acceptor->set_option(ip::tcp::acceptor::reuse_address(true));
			acceptor->bind(endpoint);
      acceptor->listen(socket_base::max_connections);
      
      acceptor->async_accept(impl->socket,boost::bind(&handle_accept,acceptor,impl));
           
	}catch(boost::system::system_error &e)
		{
			cout<<e.what()<<endl;
		}
		
		while(true)
			io_service.run_one();

		return 0;
}