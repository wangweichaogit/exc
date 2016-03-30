#include"serialize.h"
#include <iostream>
#include <boost/filesystem.hpp>
using namespace std;

#if 0
int main()
{
	CDataStream ssPeers(SER_DISK, CLIENT_VERSION);
	string str("hello world");
	ssPeers<<str;
	string name;
	ssPeers>>name;
	cout<<name.c_str()<<endl;
	return 0;
}
#endif


int main()
{
		//序列化一个字符串
	  CDataStream ssPeers(SER_DISK, CLIENT_VERSION);
	  string str("hello world");
	  ssPeers<<str;
	  
	  //序列化后码流写入二进制文件
    boost::filesystem::path pathTmp = "test.dat";
    FILE *file = fopen(pathTmp.string().c_str(), "wb");
    CAutoFile fileout = CAutoFile(file, SER_DISK, CLIENT_VERSION);
    if (!fileout)
        printf(" open failed");

    // Write and commit header, data
    try {
        fileout << ssPeers;
    }
    catch (std::exception &e) {
        printf(" I/O error");
    }
		//刷新关闭文件
    fflush(fileout);
    fileout.fclose();


	
	//存放码流的容器
  	vector<unsigned char> vchData;
		string sstr;
		//再次打开文件
	 FILE *file2 = fopen(pathTmp.string().c_str(), "rb");
   CAutoFile filein = CAutoFile(file2, SER_DISK, CLIENT_VERSION);
    if (!filein)
        printf(" open failed");
 
 		//得到文件大小
  	int fileSize = boost::filesystem::file_size(pathTmp);
  	//开辟空间
  	vchData.resize(fileSize);
   	try {
   		//读码流
       filein.read((char *)&vchData[0], fileSize);
    }
    catch (std::exception &e) {
        printf("I/O error or stream data corrupted");
    }
    filein.fclose();

	//构造序列化对象
   CDataStream ssPeers2(vchData, SER_DISK, CLIENT_VERSION);  
    try {
    	//导出string
        ssPeers2 >> sstr;
    }
    catch (std::exception &e) {
        printf(" I/O error or stream data corrupted");
    }
    cout<<sstr.c_str()<<endl;

	return 0;
}