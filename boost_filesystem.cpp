#include <iostream>
#include <boost/filesystem.hpp>
using namespace boost::filesystem;
using namespace std;
int main()
{
	path dirpath("/home/wwc/");
	path filepath("123.txt");
	path file2path("456.txt");
	path allpath;
	path despath = dirpath/file2path;
	if (is_directory(dirpath))
		allpath=dirpath/filepath;
	cout<<allpath.string()<<endl;
	cout<<allpath.filename()<<endl;
	
	if (exists(allpath))
		cout<<"file exist"<<endl;
	else
		cout<<"file not exist"<<endl;
	#if 1
	try{
		copy_file(allpath,despath);
		cout<<"success"<<endl;
	}catch(filesystem_error &e){
		cout<<e.what()<<endl;
		cout<<"failed"<<endl;

	}
#endif
	return 0;
}
