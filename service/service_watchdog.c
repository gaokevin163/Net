#include <stdio.h>
#include "skynet.h"

//watchdog
//1启动gate
//2gate启动之后有玩家链接，为每一个玩家启动一个agent
//3玩家断开链接，销毁agent


struct agent{
	int clietn_fd;
	struct agent* next;

};



//代理队列
struct agent_queue{
	struct agent* head;
	struct agent* tail;
	int cap;
};

//查找agent
static struct agent* query_agent(struct watchdog* watchdog,int client_fd){
	struct agent* agent = watchdog->queue->head;
	while(agent){
		if(agent->client_fd == client_fd){
			return agent;
		}
	}
	return NULL;
}


static void insert_agent(struct watchdog* watchdog,struct agent* agent){
	if(watchdog->queue->head == NULL){
		watchdog->queue->head = watchdog->queue->tail = agent；
		return ;
	}
	
	watchdog->queue->tail->next = agent;
	watchdog->queue->tail = agent;
	watchdog->queue->cap++;	

}


static void delete_agent(struct watchdog* watchdog,int client_fd){
	struct agent* agent = query_agent(watchdog,client_fd);
	if(agent){
		if(agent == watchdog->queue->head){
			watchdog->queue->head = agent->next;
			skynet_free(agent);
			return;
		}
		struct agent* pre_agent = watchdog->queue->head;
		while(pre_agent->next){
			if(pre_agent->next == agent){
				if(watchdog->queue->tail == agent){
					watchdog->queue->tail = pre_agent;	
				}else{
					pre_agent->next = agent->next;
				}				
				skynet_free(agent);
				return ;
			}
		}
	}
}


//开门狗中管理的代理,一个链接一个代理
struct watchdog{
	struct agent_queue* queue;
};


static void close_agent(int fd){


}




static int watchdog_cb(struct skynet_context* ctx,int type,int session,uint32_t source,const void* msg,size_t sz){
	switch(type){
		case PTYPE_TEXT:
			_ctrl(msg,sz);
			break;
		case SOCKET_CLOSE:
			//如果是socket类型的消息，那么消息一定是错误或者关闭
			
	}
}


struct watchdog* create( ){
	struct watchdog* inst = skynet_malloc(sizeof(*inst));
	inst->d=0;
	return inst;
}


//处理指令
//处理的主要指令就是open gate,但是目前不想通过指令来这么做，而是直接在启动watchdog的时候启动gate服务

static void _ctrl(const void* msg,size_t sz){
	


}


//初始化watchdog,并且启动gate服务
int watchdog_init(struct watchdog* dog,struct skynet_context* cxt,char* param){
	if(param == NULL){
		return 1;
	}	
	int len = strlen(param);
	char adr[len + 1];
	sscanf(param,"%s")		


}


