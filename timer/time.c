#include <time.h>
#include <stdio.h>

int main()
{
	time_t tm;
	time(&tm);
	
	printf("%s\n",ctime(&tm));
	struct tm * pt = gmtime(&tm);
	char buf[1024] = { 0};
	strftime(buf,sizeof(buf),"%F %R:%S",pt);
	printf("%s\n",buf);
	printf("%d:%d:%d\n",pt->tm_hour,pt->tm_min,pt->tm_sec);
	struct tm*pt1 = localtime(&tm);
	printf("%d:%d:%d\n",pt1->tm_year+1900,pt1->tm_mon+1,pt1->tm_mday);
	printf("%d:%d:%d\n",pt1->tm_hour,pt1->tm_min,pt1->tm_sec);
	return 0;
}
