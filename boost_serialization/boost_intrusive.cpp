#include<iostream>
#include<sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
class CMyData  
{  
private:  
    friend class boost::serialization::access;  
  
    template<class Archive>  
    void serialize(Archive& ar, const unsigned int version)  
    {  
        ar & _tag;  
        ar & _text;  
    }  
  
      
public:  
    CMyData():_tag(0), _text(""){}  
  
    CMyData(int tag, std::string text):_tag(tag), _text(text){}  
  
    int GetTag() const {return _tag;}  
    std::string GetText() const {return _text;}  
  
private:  
    int _tag;  
    std::string _text;  
};  

void TestArchive1()  
{  
    CMyData d1(2012, "China, good luck");  
    std::ostringstream os;  
    boost::archive::text_oarchive oa(os);  
    oa << d1;//序列化到一个ostringstream里面  
  
    std::string content = os.str();//content保存了序列化后的数据。  
  
    CMyData d2;  
    std::istringstream is(content);  
    boost::archive::text_iarchive ia(is);  
    ia >> d2;//从一个保存序列化数据的string里面反序列化，从而得到原来的对象。  
  
    std::cout << "CMyData tag: " << d2.GetTag() << ", text: " << d2.GetText() << "\n";  
}  

int main()
{
	TestArchive1();
	return 0;
}