#include <signal.h>
#include <stdio.h>
#include "door_imp.h"
#include "door_server.h"

//信号处理
int sigign(){
	struct sigaction sa;
	sa.sa_handler =SIG_IGN;
	sigaction(SIGPIPE,&sa,0);
	return 0;
	
}

//加载配置
static void load_config(struct door_config* config){
	


}



int main(int argc,char* argv[]){

	//配置文件的路径
	const char* config_file =NULL;
	if(argc > 1)
		config_file =argv[1];
	else{
		fprintf(stderr,"Need a config file, please read door wiki");
		return 1;
	
	}
	//初始化全局变量	
	door_globalinit( );
	
	//屏蔽信号
	sigign( );

	//加载配置
	struct door_config config;
	load_config(&config);
	
	//此处写入系统的默认配置
	//gao to do...

	door_start(&config);

	door_globalexit();
	
	return 0;

}












