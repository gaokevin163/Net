#include "door_module.h"
#include "spinlock.h"
#include "door_malloc.h"

#define MAX_MODULE_TYPE 32



//模块的结构
struct door_module{
	const char* name;//模块的名称
	void* module;
	door_dl_create create;//模块的创建
	door_dl_init init;//模块的初始化
	door_dl_release release;//模块的释放
	door_dl_signal signal;//模块的信号处理
};




//保存全部模块的结构体
struct modules{
	int count;//模块的数量
	struct spinlock* lock;
	const char* path;
	struct door_module m[MAX_MODULE_TYPE];
};



//静态全局的服务模块管理
static modules* M = NULL;

//初始化全局的模块
void door_module_init(char* path){
	struct modules* m = door_malloc(sizeof(*m));
	m->count =0 ;
	SPIN_INIT(m);
	m->path = door_strdup(path);
	M = m;	
}









