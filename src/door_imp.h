#ifndef _DOOR_IMP_H
#define _DOOR_IMP_H


struct door_config{
	int thread;//现成个数
	int harbor;//节点
	int profile;//计算cpu耗时
	const char* daemon;//使用daemon模式
	const char* module_path;//模块路径
	const char* bootstrap;//
	const char* logger;//
	const char* logservice;//

}




//线程的类型
#define THREAD_WORKER 0
#define THREAD_MAIN 1
#define THREAD_SOCKET 2
#define THREAD_TIMER 3
#define THREAD_MONITOR 4


void door_start(struct door_config* config);



#endif
