#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

using namespace std;

int main()
{
	boost::filesystem::path  pathlockfile="/home/wwc/.lock";
	int fd = open(pathlockfile.string().c_str(),O_RDWR|O_CREAT,0644);
	close(fd);
	static boost::interprocess::file_lock lock(pathlockfile.string().c_str());
	if (!lock.try_lock())
		cout<<"lock........"<<endl;
	while(1);
	return 0;
}
