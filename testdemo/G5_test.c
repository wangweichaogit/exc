#include <stdio.h>


static unsigned long CalcHash( char *str )
{
	unsigned long	hashval ;
	unsigned char	*puc = NULL ;
	
	hashval = 19791007 ;
	for( puc = (unsigned char *)str ; *puc ; puc++ )
	{
		hashval = hashval * 19830923 + (*puc) ;
	}
	
	return hashval;
}

int main()
{
	char * str = "hello";
	unsigned long i = CalcHash( str );
	printf("%ld\n",i);
	return 0;
}