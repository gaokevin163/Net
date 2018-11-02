#include "socket_server.h"
#include "socket_epoll.h"
#include "door_malloc.h"


#define MAX_INFO 128
#define MAX_SOCKET_P 16
#define MAX_EVENT 64
#define MIN_READ_BUFFER 64




#define SOCKET_TYPE_INVALID 0
#define SOCKET_TYPE_RESERVE 1
#define SOCKET_TYPE_PLISTEN 2
#define SOCKET_TYPE_LISTEN 3
#define SOCKET_TYPE_CONNECTING 4
#define SOKCET_TYPE_CONNECTED 5
#define SOCKET_TYPE_HALFCLOSE 6
#define SOCKET_TYPE_PACCEPT 7
#define SOCKET_TYPE_BIND 8


#define MAX_SOCKET (1<<MAX_SOCKET_P)



#define UDP_ADDRESS_SIZE 19  //ipv6 128bit + port 16bit + 1 byte type



struct write_buffer{
	struct write_buffer* next;
	void* buffer;
	char* ptr;
	int sz;
	bool userobject;
	uint8_t udp_address[UDP_ADDRESS_SIZE];
};



//读写缓存
struct wb_list{
	struct write_buffer* head;
	struct write_buffer* tail;
};



struct socket{
	uintptr_t opaque;
	struct wb_list high; //优先级
	struct wb_list low;
	int64_t wb_size;
	int fd;
	int id;
	uint16_t protocol;
	uint16_t type;
	int64_t warn_size;
	union{
		int size;
		uint8_t udp_address[UDP_ADDRESS_SIZE];
	
	}p;

};

struct socket_server{
	int recvctrl_fd;
	int sendctrl_fd;
	int checkctrl;
	poll_fd event_fd;
	int alloc_id;
	int event_n;
	int event_index;
	struct socket_object_interface soi;
	struct event ev[MAX_EVENT];
	struct socket slot[MAX_SOCKET];
	char buffer[MAX_INFO];
	uint8_t udpbuffer[MAX_UDP_PACKAGE];
	fd_set rfds;
};





//socket server的创建
struct socket_server*  socket_server_create(){
	int i;
	int fd[2];
	poll_fd efd = sp_create();
	if(sp_invalid(efd)){
		fprintf(stderr,"socket-server: create event pool faild\n");
		return NULL;
	}

	//创建双向管道
	if(pipe(fd)){
		sp_release(efd);
		fprintf(stderr,"socket-server:create socket pair faild.\n");
		return NULL;
	}

	//读端加入事件
	if(sp_add(efd,fd[0],NULL)){
		//add recvctrl_fd to event poll
		fprintf(stderr,"socket-server:cant't add server fd to event pool");
		close(fd[0]);
		close(fd[1]);
		sp_release(efd);
		return NULL;
	}
	struct socket_server* ss = door_malloc(sizeof(socket_server));
	ss->event_fd =efd;
	ss->recvctrl=fd[0];
	ss->sendctrl=fd[1];
	ss->checkctrl=1;
	
	for(i=0;i<MAX_SOCKET;i++){
		struct socket *s = &ss->slot[i];
		s->type =SOCKET_TYPE_INVALID;
		clear_wb_list(&s->high);
		clear_wb_list(&s->low);
	
	}
	ss->alloc_id = 0;
	ss->event_n = 0;
	ss->event_index = 0;
	memset(&ss->soi,0,sizeof(ss->soi));
	FD_ZERO(&ss->rfds);
	assert(ss->recvctrl_fd < FD_SETSIZE);
	return ss;


}





