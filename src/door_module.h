#ifndef _DOOR_MODULE_H
#define _DOOR_MODULE_H





//模块的结构
struct door_module;
struct modules;


//上下文
struct door_context;



//每个模块内部的函数申明
typedef void* (*door_dl_creae)(void);
typedef int (*door_dl_init)(void* instance,struct door_context* ,const char* param);
typedef void (*door_dl_release)(void * inst);
typedef void (* door_dl_signal)(void* inst,int signal);






void door_module_init(char* path);














#endif






