#include <iostream>
using namespace std;


class test
{
public:
	test(int i = 0):m(i){}
	test(const test &g):m(g.m) { cout<< "copy constructors"<<endl; }
	explicit test(test &&g):m(g.m) { g.m=-1;cout<< "move constructors"<<endl; }
	void print() { cout<<m<<endl;}
private:
	int m;
};

int main()
{
	test t;
	t.print();
	test tt(t);
	t.print();
	tt.print();
	test ttt(move(t));
	t.print();
	ttt.print();
	
	return 0;
}
