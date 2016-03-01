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
		//���л�һ���ַ���
	  CDataStream ssPeers(SER_DISK, CLIENT_VERSION);
	  string str("hello world");
	  ssPeers<<str;
	  
	  //���л�������д��������ļ�
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
		//ˢ�¹ر��ļ�
    fflush(fileout);
    fileout.fclose();


	
	//�������������
  	vector<unsigned char> vchData;
		string sstr;
		//�ٴδ��ļ�
	 FILE *file2 = fopen(pathTmp.string().c_str(), "rb");
   CAutoFile filein = CAutoFile(file2, SER_DISK, CLIENT_VERSION);
    if (!filein)
        printf(" open failed");
 
 		//�õ��ļ���С
  	int fileSize = boost::filesystem::file_size(pathTmp);
  	//���ٿռ�
  	vchData.resize(fileSize);
   	try {
   		//������
       filein.read((char *)&vchData[0], fileSize);
    }
    catch (std::exception &e) {
        printf("I/O error or stream data corrupted");
    }
    filein.fclose();

	//�������л�����
   CDataStream ssPeers2(vchData, SER_DISK, CLIENT_VERSION);  
    try {
    	//����string
        ssPeers2 >> sstr;
    }
    catch (std::exception &e) {
        printf(" I/O error or stream data corrupted");
    }
    cout<<sstr.c_str()<<endl;

	return 0;
}