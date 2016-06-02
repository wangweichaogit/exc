/************************************************************************
* Socket define
************************************************************************/

//////////////////////////////////////////////////////////////////////////
#ifndef __SOCKET_DEFINE_H__
#define __SOCKET_DEFINE_H__

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#include <fcntl.h>
#include <errno.h>
#else
#include <sys/socket.h> //socket
#include <sys/stat.h>
#include <netinet/in.h> //hton, ntoh
#include <netinet/tcp.h> 
#include <sys/types.h>  //socket
#include <sys/ioctl.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#ifndef __IOS__
#include <linux/sockios.h>
#endif
#include <netdb.h>
#include <net/if.h>
#endif

#define SOCKET_OPEN(f, t, p)				socket((f), (t), (p))
#define SOCKET_BIND(s, sin)					::bind((s), (struct sockaddr *)&(sin), sizeof(sin))
#define SOCKET_LISTEN(s, q)					listen((s), (q))
#define SOCKET_ACCEPT(s, sin, l)			accept((s), (struct sockaddr *)&(sin), (socklen_t*)&(length));
#define SOCKET_SEND_TO(s, b, n, f, a)		sendto((s), (b), (n), (f),(struct sockaddr *)&(a), sizeof(a))
#define SOCKET_SETSOCKOPT(s, x, n, v, l)	setsockopt((s), (x), (n), (v), (l))
//////////////////////////////////////////////////////////////////////////
#if defined(WIN32) 
#define SOCKET_CLOSE(s)						closesocket(s)
#define SOCKET_BLOCK(s,ind)					ioctlsocket((s), FIONBIO, (unsigned long *)&(ind))
#define SOCKET_UNBLOCK(s,ind)				ioctlsocket((s), FIONBIO, (unsigned long *)&(ind))
#else
#define SOCKET_CLOSE(s)						close(s)
#define SOCKET_BLOCK(s,ind)					ioctl((s), FIONBIO, (unsigned long)&(ind))
#define SOCKET_UNBLOCK(s,ind)				ioctl((s), FIONBIO, (unsigned long)&(ind))
#endif
//////////////////////////////////////////////////////////////////////////
#define SOCKET_ABORT(s)	
#define GET_HOST_NAME(b, l)					gethostname((b),(l))
#define STRERROR_ERRNO						strerror(errno)
#define _FD_ZERO FD_ZERO
#ifndef __IOS__
#define _FD_SET FD_SET
#endif
#define _FD_ISSET FD_ISSET
#define _FD_CLR FD_CLR				
#define OSSelect(nfds, readfds, writefds, exceptfds, timeout) select ((nfds), (readfds), (writefds), (exceptfds), (timeout))
//////////////////////////////////////////////////////////////////////////

#endif
