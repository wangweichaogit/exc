#include <stdarg.h>

int myprintf(const char *format,...)
{
	int ret = 0;
	va_list arg_ptr;
	va_start(arg_ptr,format);
	ret = vprintf(format,arg_ptr);
	va_end(arg_ptr);
}
int main()
{
	myprintf("%s,%s\n","hello","world");
	return 0;
}
