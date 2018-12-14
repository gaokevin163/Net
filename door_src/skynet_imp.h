#ifndef SKYNET_IMP_H
#define SKYNET_IMP_H

#define MAX_SERVICE 100


/*
struct skynet_config {
	int thread;
	int harbor;
	int profile;
	const char * daemon;
	const char * module_path;
	const char * bootstrap;
	const char * logger;
	const char * logservice;
};
*/


struct service{
	const char* name;
	const char* param;

};

struct skynet_config{
	int thread;
	int harbor;
	int profile;
	const char* daemon;
	const char* module_path;
	const char* logger;
	const char* logservice;
	struct service* service_config;
	int cap; 
	uint8_t tail;
	const char* master_address;
	const char* harbor_address;
};



#define THREAD_WORKER 0
#define THREAD_MAIN 1
#define THREAD_SOCKET 2
#define THREAD_TIMER 3
#define THREAD_MONITOR 4

void skynet_start(struct skynet_config * config);

#endif
