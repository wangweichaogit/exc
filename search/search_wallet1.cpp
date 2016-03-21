#include <iostream>
#include <boost/foreach.hpp>
#include <vector>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
using namespace std;

int getfilename(const string & pathname,vector<string> & vfilename)
{
	fs::path path(pathname);	
	if (!fs::exists(path))
	{
		return -1;
	}
	fs::directory_iterator end_itr;	
	for(fs::directory_iterator itr(path); itr != end_itr; itr++)
	{
		if (fs::is_regular_file(itr->status()))
		{
			if (itr->path().filename() == "wallet.dat")
			vfilename.push_back(itr->path().string());
		}
		if (fs::is_directory(itr->status()))
		{
			getfilename(itr->path().string(),vfilename);
		}
	}
	return vfilename.size();
}
int main(int argc,char *argv[])
{
	if (argc != 2)
	{
		cout<<"input path"<<endl;
		return -1;
	}
	int ret = 0;
	string pathname(argv[1]);
	vector<string> vfilename;
	ret = getfilename(pathname,vfilename);
	if (ret == -1)
	{
		cout<<"path is not exist"<<endl;
		return -2;
	}
	BOOST_FOREACH(string filename,vfilename)
	{
		cout<<filename<<endl;
	}
	return 0;
	
}
