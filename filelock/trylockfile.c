#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>

int main(void)
{
	
	int fd,status;
	fd = open(".lock",O_CREAT | O_RDONLY,0644);
	if (fd == -1)
		printf("fopen failed\n");
	status = flock(fd, LOCK_EX | LOCK_NB);
	if (status == -1)
		{
		printf("try lockfile failed\n");
		close(fd);
		return -1;
		}
	else
		printf("try lockfile success\n");
	close(fd);
	flock(fd,LOCK_UN);
	printf("unlock file\n");
	return 0;
}
