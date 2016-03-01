#include <boost/make_shared.hpp>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <iostream>

using namespace std;
using namespace boost;

#if 0
int main()
{
	typedef vector<shared_ptr<int> >  vs;
	vs v(10);
	int i= 0;
	for(vs::iterator it = v.begin(); it != v.end(); ++it)
		{
			*it = make_shared<int>(++i);
			cout<<**it<<",";
		}
		cout<<endl;
	shared_ptr<int> p = v[9];
	*p = 100;
	cout<<*v[9]<<endl;
	return 0;
}
#endif

int main()
{
	shared_ptr<int> sp(new int(10));
	assert(sp.unique());
	shared_ptr<int> sp2 = sp;
	assert(sp==sp2 && sp.use_count() == 2);
	*sp = 100;
	assert(*sp2 == 100);
	sp.reset();
	assert(!sp);
	return 0;
}
