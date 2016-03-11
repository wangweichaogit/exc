#include <iostream>
#include <boost/functional/hash.hpp>

using namespace std;
using namespace boost;


int main()
{
	hash<string> string_hash;
	size_t h = string_hash("hello world");
	cout<<h<<endl;
	return 0;
}


