#ifndef __DOOR_MALLOC_
#define __DOOR_MALLOC_


void* door_malloc(size_t size);

void* door_calloc(size_t nmemb,size_t size);


void door_free(void* ptr);
char* door_strdup(const char* str);


#endif










