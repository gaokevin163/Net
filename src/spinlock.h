

/****该锁采用两套体系分为互斥锁和非互斥锁，采用预编译的方法实现***/

#ifndef SPINLOCK_H
#define SPINLOCK_H


#define SPIN_INIT(q) spinlock_init(&(q)->lock);
#define SPIN_LOCK(q) spinlock_lock(&(q)->lock);
#define SPIN_UNLOCK(q) spinlock_unlock(&(q)->lock);
#define SPIN_DESTORY(q) spinlock_destory(&(q)->lock);


#ifndef USE_PTHREAD_LOCK

struct spinlock{
	int lock;

};


static inline void spinlock_init(struct spinlock* lock){
	lock->lock=0;

}

static inline void spinlock_lock(struct spinlock* lock){
	while(__sync_lock_test_and_set(&lock->lock,1)){}

}

static inline void spinlock_trylock(struct spinlock* lock){
	return __sync_lock_test_and_set($lock->lock,1) == 0;

}


static inline void spinlock_unlock(struct spinlock* lock){
	__sync_lock_release(&lock->lock);

}

static inline void spinlock_destory(struct spinlock* lock){
	lock->lock =0;
}


#else

#include <pthread.h>

struct spinlock{
	pthread_mutex_t lock;


};



static inline void spinlock_init(struct spinlock* lock){
	pthread_mutex_init(&lock->lock,NULL);
}


static inline void spinlock_lock(struct spinlock* lock){
	pthread_mutex_lock(&lock->lock);
}

static inline void spinlock_trylock(struct spinlock* lock){
	pthread_mutex_trylock(&lock->lock);

}

static inline void spinlock_unlock(struct spinlock* lock){
	pthread_mutex_unlock(&lock->lock);
}

static inline void spinlock_destory(struct spinlock* lock){
	pthread_mutex_destory(&lock->lock);

}

#endif

#endif

