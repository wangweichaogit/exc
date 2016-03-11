
// socketutil.h

#ifndef _socketutil_H_
#define _socketutil_H_

#ifdef __cplusplus
extern 'C'
{
#endif

#include <stdio.h>
#include <stdlib.h>

void activate_nonblock(int fd);
void deactivate_nonblock(int fd);

int read_timeout(int fd, unsigned int wait_seconds);
int write_timeout(int fd, unsigned int wait_seconds);
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);
int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);


int readn(int fd, void *buf, size_t count);
int writen(int fd, const void *buf, size_t count);
int recv_peek(int sockfd, void *buf, size_t len);
int readline(int sockfd, void *buf, size_t maxline);


#ifdef __cpluspluse
}
#endif


#endif /* _SYS_UTIL_H_ */
