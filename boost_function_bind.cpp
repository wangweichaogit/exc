#include <iostream>
#include <boost/function.hpp>
#include <boost/bind.hpp>

using namespace std;
using namespace boost;

class button
{

public:
	function<void()> onclick;
};

class player
{
public:
	void play()
	{ cout<<"play"<<endl;}
	void stop()
	{ cout<<"stop"<<endl;}
};

button playbutton ,stopbutton;
player  theplayer;

void connect()
{
	playbutton.onclick = bind(&player::play,&theplayer);
	stopbutton.onclick = bind(&player::stop,&theplayer);
}

int main()
{
	connect();
	playbutton.onclick();
	stopbutton.onclick();	
	return 0;
}
