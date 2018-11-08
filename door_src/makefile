CC = cc
CFLAGS = -pipe  -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Werror -g 
LINK = $(CC)

ALL_INCS = -I /door_src
	-I /service


DOOR_DEPS = door_src/spinlock.h\
	door_src/door_mq.h\
	door_src/door_server.h\
	door_src/door_module.h\
	door_src/door_malloc.h\
	door_src/door_socket.h\
	door_src/door_imp.h\
	door_src/socket_epoll.h\
	door_src/socket_server.h\
	door_src/malloc_hook.h


DOOR_INCS = -I /door_src




door_net: door_src/door_mq.o\
	door_src/door_main.o\
	door_src/door_module.o\
	door_src/door_server.o\
	door_src/door_socket.o\
	door_src/door_start.o\
	door_src/malloc_hook.o\
	door_src/socket_server.o
	
	${LINK} -o door_net\
	door_src/door_mq.o\
	door_src/door_main.o\
	door_src/door_module.o\
	door_src/door_server.o\
 	door_src/door_socket.o\
 	door_src/door_start.o\
 	door_src/malloc_hook.o\
 	door_src/socket_server.o\
	-ldl -lpthread\
	-Wl,-E

door_src/door_mq.o: ${DOOR_DEPS}\
	doo_src/door_mq.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}\
	-o door_src/door_mq.o\
	door_src/door_mq.c


door_src/door_main.o: ${DOOR_DEPS}\
	doo_src/door_main.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}\
	-o door_src/door_main.o\  
	door_src/door_main.c 


	
door_src/door_module.o: ${DOOR_DEPS}\        
        doo_src/door_module.c
        ${CC} -c ${CFLAGS} ${DOOR_INCS}\         
        -o door_src/door_module.o\                   
        door_src/door_module.c


door_src/door_server.o: ${DOOR_DEPS}\              
        doo_src/door_server.c
        ${CC} -c ${CFLAGS} ${DOOR_INCS}\         
        -o door_src/door_server.o\                   
        door_src/door_server.c  


door_src/door_socket.o: ${DOOR_DEPS}\              
        doo_src/door_socket.c
        ${CC} -c ${CFLAGS} ${DOOR_INCS}\         
        -o door_src/door_socket.o\                   
        door_src/door_socket.c  


door_src/door_main.o: ${DOOR_DEPS}\              
        doo_src/door_start.c
        ${CC} -c ${CFLAGS} ${DOOR_INCS}\         
        -o door_src/door_start.o\                   
        door_src/door_start.c  




door_src/malloc_hook.o: ${DOOR_DEPS}\              
        doo_src/malloc_hook.c
        ${CC} -c ${CFLAGS} ${DOOR_INCS}\         
        -o door_src/malloc_hook.o\                   
        door_src/malloc_hook.c  




door_src/socket_server.o: ${DOOR_DEPS}\              
        doo_src/socket_server.c
        ${CC} -c ${CFLAGS} ${DOOR_INCS}\         
        -o door_src/socket_server.o\                   
        door_src/socket_server.c  

clean:
	rm -f $(shell find -name "*.o")  
	rm -f door_net 


