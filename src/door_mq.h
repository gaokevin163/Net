#ifndef _DOOR_MQ_H
#define _DOOR_MQ_H

#include <stdlib.h>
#include <stdint.h>


//消息
struct door_message{
	uint32_t source;
	int session;
	void* data;
	size_t sz;

}


//各个模块的消息队列（二级消息对列）
struct message_queue;


void door_globalmq_push(struct message_queue* queue);

struct message_queue* door_globalmq_pop(void);


struct message_queue* door_mq_create(uint32_t handle);


typedef void(*message_drop)(struct door_message* , void* );

void door_mq_release(struct message_queue* q,message_drop drop_func,void* ud);

uint32_t door_mq_handle(struct message_queue* );

void door_mq_init();


void door_mq_push(struct message_queue* q,struct door_message* message);

int door_mq_pop(struct message_queue* q,struct door_message* message);


int door_mq_overload(struct message_queue* q);



#endif









