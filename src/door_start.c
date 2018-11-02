#include "door_imp.h"



#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>


//监控结构体
struct monitor{
	int count;
	struct door_monitor** m;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	int sleep;
	int quit;
};


static int SIG = 0;


static void handle_hup( int signal){
	if(signal == SIGHUP)
		SIG = 1;
}


static void start(int thread){
	pthread_t pid[thread +3];
	struct monitor* m =door_malloc(sizeof(*m));
	memset(m,0,sizeof(*m));
	m->count =thread;
	m->sleep = 0;
	
	m->m=door_malloc(thread * sizeof(struct door_monitor*));
	int i;
	for(i=0;i<thread;i++){
		m->m[i]=door_monitor_new();
	}
	if(pthread_mutex_init(&m->mutex,NULL)){
		fprintf(stderr,"Init mutex error");
		exit(1);
		
	}	

	if(pthread_cond_init(&m->cond,NULL)){
		fprintf(stderr,"Init cond error");
		exit(1);
	}	

	create_thread(&pid[0],thread_monitor,m);
	create_thread(&pid[1],thread_timer,m);
	create_thread(&pid[2],thread_socket,m);

	static int weight[] = {
		-1,-1,-1,-1,0,0,0,0,
		1,1,1,1,1,1,1,1,
		2,2,2,2,2,2,2,2,
		3,3,3,3,3,3,3,3,

	};
	struct worker_parm wp[thread];
	for(i=0;i<thread;i++){
		wp[i].m = m;
		wp[i].id = i;
		if(i < sizeof(weight)/sizeof(weight[0])){
			wp[i].weight=weight[i];
		}else{
			wp[i].weight = 0;
		}
		create_thread(&pid[i + 3],thread_worker,&wp[i]);

	}

	for(i=0;i<thread+3;i++){
		pthread_join(pid[i],NULL);

	}

	free_monitor(m);

}







void door_start(struct door_config* config){
	struct sigaction sa;
	sa.sa_handler =&handle_hup;
	sa.sa_flags=SA_RESTART;
	sigfillset(&sa.sa_mask);
	sigaction(SIGHUP,&sa,NULL);

	if(config->daemon){
		if(daemon_init(config->daemon)){
			exit(1);
		}
	}	
	
	//door_handle_init( )
	
	//初始化全局队列
	door_mq_init( );
	//初始化模块
	door_module_init(config->module_path);
	//套接字初始化
	door_socket_init( );
	door_profile_enable(config->profile);	

	struct door_conext* ctx = door_context_new(config->logservice,config->logger);
	f(ctx == NULL){
		fprintf(stderr,"Cant't launch %s service\n",config->logservice);
		exit(1);
	
	}
	//启动一些其他的服务
	//gao to do

	start(config->thread);

	door_harbor_exit();
	door_socket_free();
	if(config->daemon){
		daemon_exit(config->daemon);
	}

}




void door_harbor_init(int harbor){
	HARBOR = (unsigned int)harbor << HANDLE_REMOTE_SHIFT;

}














