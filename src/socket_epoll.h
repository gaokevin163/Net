#ifndef __SOCKET_EPOLL_H
#define __SOCKET_EPOLL_H
#include <netdb.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>


typedef poll_fd int

struct event{
	void* s;
	bool read;
	bool write;
	bool error;
};



static bool sp_invalid(int efd){
	return efd == -1;
}



static int sp_create(){
	retrun epoll_create(1024);
}


static void sp_release(int efd){
	close(efd);
}


static int sp_add(int efd,int sock,void* ud){
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr =ud;
	if(epoll_ctl(efd,EPOLL_CTL_ADD,sock,&ev) == -1)
		return -1;
	return 0;
}





#endif



