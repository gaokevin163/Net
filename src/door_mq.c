#include "door_mq.h"
#include "spinlock.h"

struct message_queue{

	//由于消息队列是多个线程在操作为了保证每次只处理一个消息，需要枷锁
	struct spinlock lock;
	//拥有此消息的服务id
	uint32_t handle;
	//消息的大小
	int cap;
	
	//头部index
	int head;
	//尾部index
	int tail;

	//是否可以释放
	int release;

	//是否在全局队列中
	int in_global;
	
	//是否过载
	int overload;

	int overload_threshold;

	struct door_message* queue;
	struct message_queue* next;

};


struct global_queue{
	struct message_queue* head;
	struct message_queue* tail;
	struct spinlock lock;
};



static global_queue* Q = NULL;

void door_mq_init(){

	struct global_queue* q =door_malloc(sizeof(*q));
	memset(q,0x00,sizeof(*q));

	SPIN_INIT(q);

	Q=q;

}


void door_globalmq_push(struct message_queue* queue){
	struct global_queue* q = Q;
	SPIN_LOCK(q->lock);
	assert(queue->next == NULL);
	if(q->tail){
		q->tail->next = queue;
		q->tail = queue;
	}
	else{
		q->head = queue;
		q->tail = q->head;	
	}
	SPIN_UNLOCK(q->lock);
}


//从头开始弹出
struct message_queue* door_globalmq_pop( ){
	struct global_queue* q = Q;
	SPIN_LOCK(q->lock);
	struct message_queue* mq = q->head;
	if(mq){
		q->head = mq->next;

		if(q->head == NULL){
			q->tail = NULL;
		}
		mq->next = NULL;
	}
	SPIN_UNLOCK(q->lock);
	return mq;
}


struct message_queue* door_mq_create( uint32_t handle){
	struct message_queue* mq = door_malloc(sizeof(*mq));
	mq->handle =handle;
	//队列消息个数
	mq->cap = 1024;
	mq->head = 0;
	mq->tail = 0;
	SPIN_INIT(mq);
	mq->queue = door_malloc( sizeof(struct door_message) * mq->cap);
	//创建的时候置1，防止刚创建完就加入全局队列
	mq->in_global = 1;
	mq->release = 0 ;
	mq->overload = 0;
	mq->overload_threshold = 1024;
	mq->next = NULL;
	return mq;
}


//消息扩展
static void expand_queue(struct message_queue* q){
	struct door_messae* new_queue =door_malloc(sizeof(struct door_message) * 2 * q->cap);
	for(int i=0;i<q->cap;i++){
		new_queue[i] = q->queue[(q->head + i) % q->cap];
	}
	q->head = 0;
	q->tail =q->cap;
	door_free(q->queue);
	q->cap *= 2;
	q->queue = new_queue;
}


void door_mq_push(struct message_queue* q,struct door_message* message){
	assert(message);
	SPIN_LOCK(q);
	q->queue[tail] = message;
	//说明tail已经是最后一个了,tail需要从头开始放
	if(++ q->tail >= q->cap)
		q->tail = 0;

	//如果cap所设定的消息值不足，则扩建
	if(q->head == q->tail)//如果这两个相等的话说明已经满了
		expand_queue(q);

	//如果不能全局队列中，加入全局队列
	if(q->in_global = 0){
		q->in_global = 1;
		door_global_push(q);
	
	}
	SPIN_UNLOCK(q);

}

//0 for success
int  door_mq_pop(struct message_queue* q,struct door_message* message){
	int ret = 1 ;
	//枷锁
	SPIN_LOCK(q);
	if(q->head != q->tail){
		*message = q->queue[q->head++];
		ret = 0; 
		if(q->head >= q->cap)
			q->head=0;
		
		int length = q->tail - q->head;
		if(length < 0){
			length +=q->cap;
		}
		while(length > q->overload_threshold){
			q->overload =length;
			q->overload_threshold * =2;
		
		}
	}else{
		q->overload_threshold = 1024;
	
	}
	if(ret){
		q->in_global = 0;
	
	}
	//处理完，解锁
	SPIN_UNLOCK(q);
	return ret;

}
int door_mq_overload(struct message_queue* q){

	return q->overload;

}


static void _release(struct message_queue* q){
	assert(q)->next == NULL;
	SPIN_DESTORY(q);
	door_free(q->queue);
	door_free(q);

}


void door_mq_release(struct message_queue* q,message_drop drop_func,void* ud){
	struct door_message message;
	while(!door_mq_pop(q,&message)){\
		drop_func(&message,ud);
	
	}
	_release(q);
}






