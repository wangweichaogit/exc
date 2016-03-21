#include <string>
#include <vector>
#include <deque>
#include <utility>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>

using namespace std;
namespace fs = boost::filesystem;
string filename;

int get_filenames(const std::string& dir, std::vector<std::string>& filenames)  
{  
    fs::path path(dir);  
    if (!fs::exists(path))  
    {  
        return -1;  
    }  
  
    fs::directory_iterator end_iter;  
    for (fs::directory_iterator iter(path); iter!=end_iter; ++iter)  
    {  
        if (fs::is_regular_file(iter->status()))  
        {  
        		if(iter->path().filename() == filename)
            filenames.push_back(iter->path().string());  
        }  
  
        if (fs::is_directory(iter->status()))  
        {  
            get_filenames(iter->path().string(), filenames);  
        }  
    }  
  
    return filenames.size();  
}  

int main(int argc,char *argv[])
{
	if (argc != 3) {
		cout<<argv[0]<<"path filename"<<endl;
		return -1;
	}

	string dir(argv[1]);
	filename =argv[2];
	vector<string> v3;
	get_filenames(dir,v3);
	BOOST_FOREACH(string filename,v3)
	{
		cout<<filename.c_str()<<endl;
	}
	return 0;	
}
