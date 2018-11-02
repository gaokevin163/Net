#include "door_socket.h"

//对socket的程序层抽象，底层直接调用socket_server.c中的接口


static struct socket_server* SOCKET_SERVER=NULL;



void door_socket_init(){
	SOCKET_SERVER =socket_server_create();

}






