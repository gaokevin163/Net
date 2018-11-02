

#include <stdlib.h>
#include "malloc_hook.h"
#include "door_malloc.h"

#define PREFIX_SIZE sizeof(struct memcookie)




struct mem_cookie{
	uint32_t handle;
#ifdef MEMORY_CHECK
	uint32_t dogtag;
#endif	
};



inline static void* fill_prefix(void* ptr){
	//uint32_t handle = door



}


inline static void* clean_prefix(void* ptr){
	




}

static void malloc_oom(size_t size){
	fprintf(stderr,"xmalloc:Out of memory trying to allocate %zu bytes\n",size);
	fflush(stderr);
	abort();
}


void* door_malloc(size_t size){
	void* ptr =je_malloc(size + PREFIX_SIZE);	
	if(!ptr) malloc_oom(size);
	return fill_prefix(ptr);
}

void* door_realloc(void* ptr,size_t size){
	if(ptr == NULL) return door_malloc(size);
	void* newptr = je_malloc(rawptr,size + PREFIX_SIZE);
	if(!newptr) malloc_oom(size);
	return fill_prefix(newptr);
}



void* door_free(void* ptr){
	if(ptr == NULL) return;
	void* rawptr =clean_prefix(ptr);
	je_free(rawptr);
}





