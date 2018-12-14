#include "skynet.h"

#include "skynet_imp.h"
#include "skynet_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>

/*
static int
optint(const char *key, int opt) {
	const char * str = skynet_getenv(key);
	if (str == NULL) {
		char tmp[20];
		sprintf(tmp,"%d",opt);
		skynet_setenv(key, tmp);
		return opt;
	}
	return strtol(str, NULL, 10);
}

static int
optboolean(const char *key, int opt) {
	const char * str = skynet_getenv(key);
	if (str == NULL) {
		skynet_setenv(key, opt ? "true" : "false");
		return opt;
	}
	return strcmp(str,"true")==0;
}

static const char *
optstring(const char *key,const char * opt) {
	const char * str = skynet_getenv(key);
	if (str == NULL) {
		if (opt) {
			skynet_setenv(key, opt);
			opt = skynet_getenv(key);
		}
		return opt;
	}
	return str;
}
*/
/*
static void
_init_env(lua_State *L) {
	lua_pushnil(L); //  first key 
	while (lua_next(L, -2) != 0) {
		int keyt = lua_type(L, -2);
		if (keyt != LUA_TSTRING) {
			fprintf(stderr, "Invalid config table\n");
			exit(1);
		}
		const char * key = lua_tostring(L,-2);
		if (lua_type(L,-1) == LUA_TBOOLEAN) {
			int b = lua_toboolean(L,-1);
			skynet_setenv(key,b ? "true" : "false" );
		} else {
			const char * value = lua_tostring(L,-1);
			if (value == NULL) {
				fprintf(stderr, "Invalid config table key = %s\n", key);
				exit(1);
			}
			skynet_setenv(key,value);
		}
		lua_pop(L,1);
	}
	lua_pop(L,1);
}
*/


static struct skynet_config* config = NULL;


static int optint(const char* value,int opt){
	if(!value){
		return opt;
	}
	return strtol(value,NULL,10);
}


static const char* optstring(const char* value,const char* opt){
	if(value){
		if(!strcmp(value,"NULL")){
			return opt;
		}
		char* ret = skynet_malloc(strlen(value) + 1);
		memcpy(ret,value,strlen(value) + 1);
		return ret;
	}
	return opt;
}






//简单的text配置,配置字符之间不要有空格
void init_config(const char* config_path){
	config = skynet_malloc(sizeof(struct skynet_config));
	config->cap = MAX_SERVICE;	
	config->tail = 0;
	config->service_config = skynet_malloc(sizeof(struct service) * config->cap);
	//thread
	FILE* f = fopen(config_path,"r");
	if(f){
		char buf[1024];
		while(fgets(buf,1024,f)){
			char* str_tem = strchr(buf,'=');
			if(!str_tem){
				memset(buf,0,1024);
				continue;
			}
			char key[str_tem - buf + 1];
			strncpy(key,buf,(str_tem-buf));
			key[str_tem-buf] ='\0';
			char* value = strtok(str_tem + 1 ,";");
			if(!strcmp(key,"thread")){
				//char* value = strtok(str_tem + 1 ,';');
				config->thread = optint(value,8);
			}else if(!strcmp(key,"cpath")){			
				config->module_path=optstring(value,"./cservice/?.so");
			}else if(!strcmp(key,"harbor")){
				config->harbor=optint(value,1);
			}else if(!strcmp(key , "daemon")){
				config->daemon = optstring(value,NULL);
			}else if(!strcmp(key, "logger")){
				config->logger=optstring(value,NULL);
			}else if(!strcmp(key , "logservice")){
				config->logservice = optstring(value,"logger");
			}else if(!strcmp(key,"profile")){
				config->profile = optint(value,1);
			}else{
				if(!strcmp(key , "service")){
					//分析服务的参数
					char* name = strtok(value,"-");
					char* param = strchr(value,'=');
					struct service s;
					s.name =optstring(name,"");
					config->service_config[config->tail].name=s.name;
					if(param){	
						s.param=strtok(param + 1 ,";");
						s.param =optstring(s.param,NULL);
						config->service_config[config->tail].param =s.param;
					}else{
						config->service_config[config->tail].param=NULL;
					}
					
					config->tail++;
				}
			}
		memset(buf,0,1024);
		}
	}
}




int sigign() {
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, 0);
	return 0;
}

/*
static const char * load_config = "\
	local result = {}\n\
	local function getenv(name) return assert(os.getenv(name), [[os.getenv() failed: ]] .. name) end\n\
	local sep = package.config:sub(1,1)\n\
	local current_path = [[.]]..sep\n\
	local function include(filename)\n\
		local last_path = current_path\n\
		local path, name = filename:match([[(.*]]..sep..[[)(.*)$]])\n\
		if path then\n\
			if path:sub(1,1) == sep then	-- root\n\
				current_path = path\n\
			else\n\
				current_path = current_path .. path\n\
			end\n\
		else\n\
			name = filename\n\
		end\n\
		local f = assert(io.open(current_path .. name))\n\
		local code = assert(f:read [[*a]])\n\
		code = string.gsub(code, [[%$([%w_%d]+)]], getenv)\n\
		f:close()\n\
		assert(load(code,[[@]]..filename,[[t]],result))()\n\
		current_path = last_path\n\
	end\n\
	setmetatable(result, { __index = { include = include } })\n\
	local config_name = ...\n\
	include(config_name)\n\
	setmetatable(result, nil)\n\
	return result\n\
";
*/
int
main(int argc, char *argv[]) {

	const char * config_file = NULL ;
	if (argc > 1) {
		config_file = argv[1];
	} else {
		fprintf(stderr, "Need a config file. Please read skynet wiki : https://github.com/cloudwu/skynet/wiki/Config\n"
			"usage: skynet configfilename\n");
		return 1;
	}

	//luaS_initshr();
	skynet_globalinit();
	//skynet_env_init();

	sigign();

	//struct skynet_config config;

	//struct lua_State *L = luaL_newstate();
	//luaL_openlibs(L);	// link lua lib

	//int err =  luaL_loadbufferx(L, load_config, strlen(load_config), "=[skynet config]", "t");
	//assert(err == LUA_OK);
	//lua_pushstring(L, config_file);

	//err = lua_pcall(L, 1, 1, 0);
	//if (err) {
	//	fprintf(stderr,"%s\n",lua_tostring(L,-1));
	//	lua_close(L);
	//	return 1;
	//}
	//_init_env(L);

	//config.thread =  optint("thread",8);
	//config.module_path = optstring("cpath","./cservice/?.so");
	//config.harbor = optint("harbor", 1);
	//config.bootstrap = optstring("bootstrap","snlua bootstrap");
	//config.daemon = optstring("daemon", NULL);
	//config.logger = optstring("logger", NULL);
	//config.logservice = optstring("logservice", "logger");
	//config.profile = optboolean("profile", 1);

	//lua_close(L);
	//解析配置文件	
	init_config(config_file);
	int i;
	for(i = 0; i< config->tail;i++){
		fprintf(stderr,"launch service %s\n",config->service_config[i].name);
	}
	fprintf(stderr,"module_path %s\n",config->module_path);
	//config.thread =8;
	//config.module_path = "./cservice/?.so";
	//config.harbor = 1;
	//config.daemon = NULL;
	//config.logger = NULL;
	//config.logservice = "logger";
	//config.profile = 1;	

	skynet_start(config);
	skynet_globalexit();
	//luaS_exitshr();

	return 0;
}
