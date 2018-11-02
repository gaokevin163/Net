#include "door_server.h"
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#include "door_imp.h"
#include "door_module.h"
#include "door_mq.h"
#include "door_atomic.h"



struct door_context{
	void* instance;
	struct door_module* mod;
	void* cb_ud;
	door_cb cb;
	struct message_queue* queue;
	FILE* logfile;
	uint64_t cpu_cost;
	uint64_t cpu_start;
	char result[32];
	uint32_t handle;
	int session_id;
	int ref;
	int message_count;
	bool init;
	bool endless;
	bool profile;

};








struct door_node{
	int total;//节点总数
	int init;//是否初始化
	uint32_t monitor_exit;
	pthread_key_t handle_key;
	bool profile;

};


static struct door_node G_NODE;

int door_context_total(){
	return G_NODE.total;
}



static void context_inc(){

	ATOM_DEC(&G_NODE.tatal);

}

static void context_dec(){
	ATOM_DEC(&G_NODE.total);
}

uint32_t door_current_handle(void){
	if(G_NODE.init){
		void* handle = pthread_getspecific(G_NODE.handle_key);
		return (uint32_t)(uintptr_t)handle;
	
	}else{
		uint32_t v= (uint32_t)(-THREAD_MAIN);
		return v;
	
	}

}

static void id_to_hex(char* str,uint32_t id){
	int i;
	static char hex[16] ={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	str[0]=':';
	for(i=0;i<8;i++){
		str[i+1] =hex[(id >> ((7 - i) * 4)) & 0xf];
	
	}
	str[9] ='\0';

}

struct drop_t{
	uint32_t handle;

};


static void drop_message(str){
	struct drop_t *d =ud;
	door_free(msg->data);
	uint32_t source = d->handle;
	assert(source);
	//report error to the message source
	door_send(NULL,source,msg->source,PTYPE_ERROR,0,NULL,0);

}




int door_context_newsession(struct door_context* ctx){

	int session = ++ ctx->session_id;
	if(session <=0){
		ctx->session_id =1;
		return 1;
	
	}
	return session;
}



void door_context_grab(struct door_context* ctx){
	ATOM_INC(&ctx->ref);

}


void door_context_reserve(struct door_context* ctx){
	door_context_grab(ctx);
	context_dec();
}



void door_globalinit( ){
	G_NODE.total = 0;
	G_NODE.init = 1;
	G_NODE.monitor_exit = 0;
	if(pthread_key_create(&G_NODE.handle_key,NULL)){
		fprintf(stderr,"pthread_key_create failed");
		exit(1);
	
	}
	
	//
	door_initthread(THREAD_MAIN);

}



void door_globalexit(){
	pthread_key_delete(G_NODE.handle_key);

}





void door_initthread(int m){
	uintprt_t v =(uint32_t)(-m);
	pthread_setspecific(G_NODE.handle_key,(void*) v);

}

void door_profile_enbale(int enable){
	G_NODE.profile =(bool)enable;

}

struct door_context* door_context_new(const char* name,const char* param){
	struct door_module* mod=door_module_query(name);
	if(mod == NULL)
		return NULL;
	void* inst =door_module_instance_create(mod);
	if(inst == NULL)
		return NULL;
	struct door_context* ctx = door_malloc(sizeof(*ctx));

	ctx->mod = mod;
	ctx->instance = inst;
	ctx->ref =2;
	ctx->cb =NULL;
	ctx->cb_ud = NULL;
	ctx->session_id = 0;
	ctx->logfile =NULL:
	ctx->init = false;
	ctx->endless =false;
	
	ctx->cpu_cost =0;
	ctx->cpu_start = 0;
	ctx->message_count = 0;
	ctx->profile =G_NODE.profile;
	ctx->handle = 0;
	ctx->handle = door_handle_register(ctx);
	struct message_queue* queue = ctx->queue = door_mq_create(ctx->handle);
	context_inc();


	int r=door_module_instance_init(mod,inst,ctx,param);
	if(r==0){
		struct door_context* ret = door_context_release(ctx);
		if(ret)
			ctx->init =true;
		door_globalmq_push(queue);
		if(ret){
			door_error(ret,"launch %s %s",name,param?param:"");
		}
		return ret;
	}else{
		door_error(ctx,"FAILD launch %s",name);
		uint32_t handle =ctx->handle;
		door_context_release(ctx);
		door_handle_retire(handle);
		struct drop_t d ={handle};
		door_mq_release(queue,drop_message,&d);
		return NULL;
	}

}


static void delete_context(struct door_context* ctx){
	if(ctx->logfile){
		fclose(ctx->logfile);
	}
	door_module_instance_release(ctx->mod,ctx->instance);
	door_mq_mark_release(ctx->queue);
	door_free(ctx);
	context_dec();

}

struct door_context* door_context_release(struct door_context* ctx){
	if(ATOM_DEC(&ctx->ref) == 0){
		delete_context(ctx);
		return NULL;
	}
	return ctx;
}


int door_context_push(uint32_t handle,struct door_message* message){

	struct door_context* ctx =door_handle_grab(handle);
	if(ctx == NULL)
		return -1;
	door_mq_push(ctx->queue,message);
	door_context_release(ctx);
	return 0;
}


void door_context_endless(uint32_t handle){
	struct door_context* ctx = door_handle_grab(handle);
	if(ctx == NULL)
		return;
	ctx->endless =true;
	door_context_release(ctx);
}

int door_isremote(struct door_context* ctx,uint32_t handle,int* harbor){
	int ret =door_harbor_message_isremote(handle);
	if(harbor){
		*harbor = (int)(handle >> HANDLE_REMOTE_SHIFT);
	}
	return ret;
}

static void dispatch_message(struct door_context* ctx,struct door_message* msg){
	assert(ctx->init);

	pthread_setspecific(G_NODE.handle_key,(void*)(uintptr_t)(ctx->handle));
	int type = msg->sz >> MESSAGE_TYPE_SHIFT;
	size_t sz = msg->sz & MESSAGE_TYPE_MASK;	if(ctx->logfile){
		door_log_output(ctx->logfile,msg->source,type,msg->session,msg->data,sz);
	}
	++ctx->message_count;
	int reserve_msg;
	if(ctx->profile){
		ctx->cpu_start = door_thread_time();		reserve_msg =ctx->cb(ctx,ctx->cb_ud,type,msg->session,msg->source,msg->data,sz);	uint64_t cost_time =door_thread_time() -ctx->cpu_start;
		ctx->cpu_cost += cost_time;	
	}else{
		reserve_msg = ctx->cb(ctx,ctx->cb_ud,type,msg->session,msg->dource,msg->data,sz);
	}

	if(!reserve_msg){
		door_free(msg->data);
	}
}


void door_context_dispatchall(struct door_context* ctx){
	struct door_message msg;
	struct message_queue* q=ctx->queue;
	while(!door_mq_pop(q,&msg)){
		dispatch_message(ctx,&msg);
	}


}



struct message_queue* door_context_message_dispatch(struct door_monitor* sm,struct message_queue* q,int weight){
	if(q == NULL){
		q =door_globalmq_pop();
		if(q == NULL)
			return NULL;
	}
	uint32_t handle = door_mq_handle(q);
	struct door_context* ctx = door_handle_grab(handle);
	if(ctx == NULL){
		struct drop_t d ={handle};
		door_mq_release(q,drop_message,&d);
		return door_globalmq_pop();
	}
	int i,n=1;
	struct door_message msg;
	for(i=0;i<n;i++){
		if(door_mq_pop(q,&msg)){
			door_context_release(ctx);
			return door_globalmq_pop();
		}else if(i==0 && weight > =0 ){
			n =door_mq_length(q);
			n >>=weight;
		}
		int overload =door_mq_overload(q);
		if(overload){
			door_error(ctx,"May pverload,message queue length =%d",overload);
		}
		//door_monitor_trigger(sm,msg.source,handle);
		if(ctx->cb == NULL){
			door_free(msg.data)
		}else{
			dispatch_message(ctx,&msg);
		}
		//door_monitor_trigger(sm,0,0);
	}

	assert(q == ctx->queue);
	struct message_queue* nq =door_globalmq_pop();
	if(nq){
		door_globalmq_push(q);
		q =nq;
	}
	door_context_release(ctx);
	return q;
}


static void copy_name(char name[GLOBALNAME_LENGTH],const char* addr){
	int i;
	for(i =0;i<GLOBALNAME_LENGTH && addr[i];i++){
		name[i] =addr[i];
	}
	for(;i<GLOBALNAME_LENGTH;i++){
		name[i]='\0';
	}
}



//door command


struct command_func{
	const char* name;
	const char* (*func)(struct door_context* context,const char* param);

}



static const char*
cmd_timeout(struct door_context* context,const char* param){
	char* session_ptr = NULL;
	int ti =strtol(param,&session_ptr,10);
	int session = door_context_newsession(context);
	door_timeout(context->handle,ti,session);
	sprintf(context->result,"%d",session);
	return context->result;

}


static const char* cmd_reg(struct door_context* context,const char* param){
	if(param == NULL && param[0] == '\0'){
		sprintf(context->result,"%x",context->handle);
		return context->result;
	}else if(param[0] == '.'){
		return door_handle_namehandle(context->handle,param);
	
	}else{
		door_error(context,"Cant't register global name %s in c",param);
		return NULL;	
	}

}


static const char* cmd_query(struct door_context* context,const char* param){
	if(param[0] == '.'){
		uint32_t handle = door_handle_findname(param + 1);
		if(handle){
			sprintf(context->result,"%x",handle);
			return context->result;
		
		}
	
	}
	return NULL;

}


static const char* cmd_name(struct door_context* context,const char* param){
	int size = strlen(param);
	char name[size+1] ;
	char handle[size+1];
	sscanf(param,"%s %s",name,handle);
	if(handle[0] != ':'){
		return NULL;
	
	}
	uint32_t handle_id =strtoul(handle +1,NULL,16);
	if(handle_id == 0)
		return NULL;
	if(name[0] == '.')
		return door_handle_namehandle(handle_id,name +1);
	else
		door_error(context,"Can't set global name %s in c",name);
	return NULL;

}



static const char* cmd_exit(struct door_context* context,const char* param){
	handle_exit(context,0);
	return NULL;
}


static uint32_t tohandle(struct door_context* context,const char* param){
	uint32_t handle = 0;
	if(param[0] == ':'){
		handle = strtoul(param +1,NULL,16);
	}else if(param[0] == '.'){
		handle =door_handle_findname(param +1);
	}else{
	
		door_error(context,"Can't convert %s to handle",param);
	}
	return handle;
}


static const char* cmd_kill(struct door_context* context,const char* param){
	uint32_t handle = tohandle(context,param);
	if(handle){
		handle_exit(context,handle);
	}
	return NULL;
}

static const char* cmd_launch(struct door_context* context,const char* param){
	size_t sz = strlen(param);
	char tmp[sz +1];
	strcpy(tmp,param);
	char* args =tmp;
	char* mod = strsep(&args,"\t\r\n");
	args =strsep(&args,"\r\n");
	struct door_context* inst =door_context_new(mod,args);
	if(inst == NULL){
		return NULL;
	}else{
		id_to_hex(context->result,inst->handle);
		return context->result;
	}
}



static struct command_func cmd_funcs[] = {
	{"TIMEOUT",cmd_timeout},
	{"REG",cmd_reg},
	{"QUERY",cmd_query},
	{"NAME",cmd_name},
	{"EXIT",cmd_exit},
	{"KILL",cmd_kill},
	{"LAUNCH",cmd_launch},
	{NULL,NULL},

};


const char* door_command(struct door_context* context,const char* cmd,const char* param){
	struct command_func* method =&cmd_funcs[0];
	while(method->name){
		if(strcmp(cmd,method->name)==0){
			return method->func(context,param);
		}
		++method;
	}
	return NULL;
}

static void _filter_args(struct door_context* context,int type,int* session,void** data,size_t* sz){
	int needcopy  = !(type & PTYPE_TAG_DONTCOPY);
	int allocsession = type & PTYPE_TAG_ALLOCSESSION;
	type &=0xff;

	if(allocsession){
		assert(*session == 0);
		*session = door_context_newsession(context);
	}
	if(needcopy && *data){
		char* msg = door_malloc(*sz +1);
		memcpy(msg,*data,*sz);
		msg[*sz] = '\0';
		*data =msg;
	}
	*sz |=(size_t)type << MESSAGE_TYPE_SHIFT;
}

int door_send(struct door_context* context,uint32_t source,uint32_t destination,int type,intsession,void* data,size_t sz){
	if(sz & MESSAGE_TYPE_MASK != sz){
		door_error(context,"the message to %x is too large",destination);
		if(type & PTYPE_TAG_DONTCOPY){
			door_free(data);
		}
		return -1;
	}
	_filter_args(context,type,&session,(void**)&data,&sz);
	if(source == 0){
		source =context->handle;
	}
	if(destination  == 0){
		return session;
	}
	//判断是否是远程节点
	if(door_harbor_message_isremote(destination)){
		struct remote_message* rmsg = door_malloc(sizeof(*rmsg));
		rmsg->destination.handle = destination;
		rmsg->message =data;
		rmsg->sz = sz;
		door_harbor_send(rmsg,source,session);
	}else{
		struct door_message	smsg;
		smsg.source = source;
		smsg.session = session;
		smsg.data = data;
		smsg.sz =sz;
		if(door_context_push(destination,&smsg)){
			door_free(data);
			return -1;
		}

	}
	return session;

}



uint32_t door_context_handle(struct door_context* ctx){
	return ctx->handle;
}

void door_callback(struct door_context* context,void* ud,door_cb cb){
	context->cb =cb;
	context->cb_ud =ud;
}


void door_context_send(sruct door_context* ctx,void* msg,size_t sz,uint32_t source,int type,int session){
	struct door_message smsg;
	smsg.source =source;
	smsg.session =session;
	smsg.data =msg;
	smsg.sz = sz | (size_t)type << MESSAGE_TYPE_SHIFT;

	door_mq_push(ctx->queue,&smsg);

}































