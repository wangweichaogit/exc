/****************************************************************************    
  program:      proxyd                                                           
  module:       proxyd.c                                                         
  summary:      provides proxy tcp service for a host on an isolated network.    
  programmer:   Carl Harris (ceharris@vt.edu)                                    
  date:         22 Feb 94                                                        
  description:                                                                   
        This code implements a daemon process which listens for tcp connec-      
        tions on a specified port number.  When a connection is established,     
        a child is forked to handle the new client.  The child then estab-       
        lishes a tcp connection to a port on the isolated host.  The child       
        then falls into a loop in which it writes data to the isolated host      
        for the client and vice-versa.  Once a child has been forked, the        
        parent resumes listening for additional connections.                     
        The name of the isolated host and the port to serve as proxy for,        
        as well as the port number the server listen on are specified as         
        command line arguments.                                                  
 ****************************************************************************/  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


#define TCP_PROTO "tcp"

int proxy_port; //port to listen for proxy connections on
struct sockaddr_in hostaddr; //host addr assembled form gethostbyname()

char **sys_myerrlist;

void parse_args(int argc, char **argv);
void daemonize(int servfd);
void do_proxy(int usersockfd);
void reap_status(void);
void errorout(char *msg);

typedef void sigfunc(int );//sigfunc可以声明一个函数类型
/*description: 　 Main level driver. After daemonizing the process,
*  a socket is opened to listen for connections on the proxy port, 
*  connections are accepted and children are spawned to handle each 
*  new connection.
*/

int main(int argc, char **argv)
{
	socklen_t clilen;
	int childpid;
	int sockfd, newsockfd;
	struct sockaddr_in servaddr, cliaddr;
	parse_args(argc, argv);//prepare an address struct to listen for connect
	bzero((char*)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = proxy_port;
	//get asocket ..
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
	  fputs("failed to crate server socket\r\n", stderr);
	  exit(1);
	}
	// and bind our address and port to it
	if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{
	  fputs("failed to bind server socket to specified/r/n", stderr);
	  exit(1);
	}
	// get ready to accept with at most 5 clients waitting to connect
	listen(sockfd, 5);
	// turn ourselves into daemon
	daemonize(sockfd);
	//fall into a loop to accept new connections and spawn children
	while(1)
	{
		//accept the next connection
		clilen = sizeof(cliaddr);
		newsockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen);
		if(newsockfd <0 && errno == EINTR)
		  continue;
		//a signal might interrupt our accept call
		else if(newsockfd < 0)
		  errorout("failed to accept connection");//sth quiet amiss--kill server
		//fork a child to handle this connection
		if((childpid = fork()) == 0)
		{
			close(sockfd);// inherit
			do_proxy(newsockfd);
			exit(0);
		}
		// if fork falied the connection is silently dropped --oops
		close(newsockfd);
	}
	return 0;
}

//parse the command line args
void parse_args(int argc, char **argv)
{
	int i;
	struct hostent *hostp;//host entry
	struct servent *servp;
	unsigned long inaddr;
	struct {
		char proxy_port[16];
		char isolate_host[64];
		char service_name[32];
	}pargs;
	if(argc < 4)
	{
	  printf("usage:%s<proxy-port> <host> <service-name|port-number> \r\n", argv[0]);
	  exit(1);
	}
	strcpy(pargs.proxy_port, argv[1]);
	strcpy(pargs.isolate_host, argv[2]);
	strcpy(pargs.service_name, argv[3]);
	for(i=0; i<strlen(pargs.proxy_port); i++)
	  if(!isdigit(*(pargs.proxy_port+i)))
		break;
	if(i ==  strlen(pargs.proxy_port))
	  proxy_port = htons(atoi(pargs.proxy_port));//port short
	else
	{
	  printf("invalid proxy port\r\n", pargs.proxy_port);
	  exit(0);
	}
	bzero(&hostaddr, sizeof(hostaddr));
	hostaddr.sin_family = AF_INET;
	if((inaddr= inet_addr(pargs.isolate_host))!= INADDR_NONE)
	  bcopy(&inaddr, &hostaddr.sin_addr, sizeof(inaddr));
	else if((hostp == gethostbyname(pargs.isolate_host)) != NULL)
	  bcopy(hostp->h_addr,&hostaddr.sin_addr,hostp->h_length);
	else
	{
		printf("%s unknow host \r\n", pargs.isolate_host);
		exit(1);
	}
	if((servp = getservbyname(pargs.service_name, TCP_PROTO)) != NULL)
	  hostaddr.sin_port = servp->s_port;
	else if(atoi(pargs.service_name) >0)
	  hostaddr.sin_port = htons(atoi(pargs.service_name));
	else
	{
		printf("%s invalid unknow service or port no.\r\n", pargs.service_name);
		exit(1);
	}
}

/*detach the server process from the current context, creating a pristine,
* predictable environment in which it will */

void daemonize(int servfd)
{
	int childpid, fd, fdtablesize;
	// ignore terminal I/O,stop signals
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	/* fork to put us in the background (whether or not the user
	  specified '&' on the command line */
	if((childpid=fork()) < 0)
	{
		fputs("failed to fork first child/r/n",stderr);
		exit(1);
	}
	else if(childpid >0)
	  exit(0);// terminate parent, continue child
	//dissociate from process group
	if (setpgrp() < 0) 
	{
		fputs("failed to become process group leader/r/n",stderr);
		exit(1);
	}
	/* lose controlling terminal */
	if ((fd = open("/dev/tty", O_RDWR)) >= 0)
	{
		ioctl(fd,TIOCNOTTY,NULL);
		close(fd);
	}
	/* close any open file descriptors */
	for(fd = 0, fdtablesize = getdtablesize(); fd < fdtablesize; fd++)
	  if (fd != servfd)
		close(fd);
	/* set working directory to allow filesystems to be unmounted */
	chdir("/");
	/* clear the inherited umask */
	umask(0);
	/* setup zombie prevention */
	signal(SIGCLD,(sigfunc *)reap_status);
}

//handle a SIGCLD signal by reaping the exit status of the perished child, 
//and discarding it.
void reap_status()
{
	int pid;
	union wait status;
	while ((pid = wait3(&status,WNOHANG,NULL)) > 0)
	  ; /* loop while there are more dead children */
}

//does the actual work of virtually connecting a client to the telnet
//service on the isolated host.

void do_proxy (int usersockfd)
{
	int isosockfd;
	fd_set rdfdset;
	int connstat;
	int iolen;
	char buf[2048];
	/* open a socket to connect to the isolated host */
	if ((isosockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	  errorout("failed to create socket to host");
	/* attempt a connection */
	connstat = connect(isosockfd,(struct sockaddr *) &hostaddr, sizeof(hostaddr));
	switch (connstat)
	{
		case 0:
			break;
		case ETIMEDOUT:
		case ECONNREFUSED:
		case ENETUNREACH:
		    strcpy(buf,sys_myerrlist[errno]);
		    strcat(buf,"\r\n"); 
		    write(usersockfd, buf, strlen(buf));
		    close(usersockfd);
	 	    exit(1);
                    /* die peacefully if we can't establish a connection */
		    break;
		default:
		        errorout("failed to connect to host");
	}
/* now we're connected, serve fall into the data echo loop */
	while (1)
	{
		/* Select for readability on either of our two sockets */
		FD_ZERO(&rdfdset);
		FD_SET(usersockfd,&rdfdset);
		FD_SET(isosockfd,&rdfdset);
		if (select(FD_SETSIZE,&rdfdset,NULL,NULL,NULL) < 0)
		errorout("select failed");
		/* is the client sending data? */
		if (FD_ISSET(usersockfd,&rdfdset)) 
		{
			if ((iolen = read(usersockfd,buf,sizeof(buf))) <= 0)
			  break; /* zero length means the client disconnected */
			write(isosockfd,buf,iolen);
			/* copy to host -- blocking semantics */
		}
		/* is the host sending data? */
		if(FD_ISSET(isosockfd,&rdfdset)) 
		{
			if((iolen = read(isosockfd,buf,sizeof(buf))) <= 0)
			  break; /* zero length means the host disconnected */
			write(usersockfd,buf,iolen);
			/* copy to client -- blocking semantics */
		}
	}
	/* we're done with the sockets */
	close(isosockfd);
	close(usersockfd);
}

//displays an error message on the console and kills the current process.
void errorout (char *msg)
{
	FILE *console;
	console = fopen("/dev/console","a");
	fprintf(console,"proxyd: %s\r\n",msg);
	fclose(console);
	exit(1);
}
