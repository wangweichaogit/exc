#include <boost/make_shared.hpp>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <boost/weak_ptr.hpp>
using namespace std;
using namespace boost;

int main()
{
	shared_ptr<int> sp(new int(10));
	cout<<sp.use_count()<<endl;
	
	//��shared_ptr����weak_ptr
	weak_ptr<int> wp(sp);
	cout<<wp.use_count()<<endl;
	
	if ( !wp.expired())
	{
		//��weak_ptr���shared_ptr
		shared_ptr<int> sp2 = wp.lock(); 
		*sp2 = 100;
		cout<<wp.use_count()<<endl;
	}	
			cout<<wp.use_count()<<endl;
			return 0;
}
