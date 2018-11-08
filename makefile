CC = cc
CFLAGS = -pipe  -O -W -Wall -Wpointer-arith -Wno-unused-parameter  -g 
LINK = $(CC)

ALL_INCS = -I /door_src -I /jemalloc/include/jemalloc

LD_FLAGS = -L./jemalloc/lib


DOOR_DEPS = door_src/atomic.h  \
door_src/malloc_hook.h  \
door_src/rwlock.h  \
door_src/skynet_daemon.h  \
door_src/skynet.h  \
door_src/skynet_handle.h  \
door_src/skynet_harbor.h  \
door_src/skynet_imp.h  \
door_src/skynet_log.h  \
door_src/skynet_malloc.h  \
door_src/skynet_module.h  \
door_src/skynet_monitor.h  \
door_src/skynet_mq.h  \
door_src/skynet_server.h  \
door_src/skynet_socket.h  \
door_src/skynet_timer.h  \
door_src/socket_epoll.h  \
door_src/socket_kqueue.h  \
door_src/socket_poll.h  \
door_src/socket_server.h  \
door_src/spinlock.h


DOOR_INCS = -I ./door_src -I ./jemalloc/include/jemalloc




door_net:door_src/malloc_hook.o  \
	door_src/skynet_daemon.o  \
	door_src/skynet_handle.o  \
	door_src/skynet_harbor.o  \
	door_src/skynet_log.o  \
	door_src/skynet_module.o  \
	door_src/skynet_monitor.o  \
	door_src/skynet_mq.o  \
	door_src/skynet_server.o  \
	door_src/skynet_socket.o  \
	door_src/skynet_timer.o  \
	door_src/socket_server.o  \
	door_src/skynet_error.o  \
	door_src/skynet_start.o  \
	door_src/skynet_main.o
	${LINK} -o door_net  \
	door_src/malloc_hook.o  \
	door_src/skynet_daemon.o  \
	door_src/skynet_handle.o  \
	door_src/skynet_harbor.o  \
	door_src/skynet_log.o  \
	door_src/skynet_module.o  \
	door_src/skynet_monitor.o  \
	door_src/skynet_mq.o  \
	door_src/skynet_server.o  \
	door_src/skynet_socket.o  \
	door_src/skynet_timer.o  \
	door_src/socket_server.o  \
	door_src/skynet_error.o  \
	door_src/skynet_start.o  \
	door_src/skynet_main.o  \
	-ldl -lpthread  ${LD_FLAGS} -ljemalloc  \
	-Wl,-E

door_src/malloc_hook.o: ${DOOR_DEPS}  \
	door_src/malloc_hook.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}  \
	-o door_src/malloc_hook.o  \
	door_src/malloc_hook.c

	
door_src/skynet_daemon.o: ${DOOR_DEPS}  \
	door_src/skynet_daemon.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}  \
	-o door_src/skynet_daemon.o  \
	door_src/skynet_daemon.c


door_src/skynet_handle.o: ${DOOR_DEPS}  \
	door_src/skynet_handle.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}  \
	-o door_src/skynet_handle.o  \
	door_src/skynet_handle.c  


door_src/skynet_harbor.o: ${DOOR_DEPS}  \
	door_src/skynet_harbor.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}  \
	-o door_src/skynet_harbor.o  \
	door_src/skynet_harbor.c  


door_src/skynet_log.o: ${DOOR_DEPS}  \
	door_src/skynet_log.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}  \
	-o door_src/skynet_log.o  \
	door_src/skynet_log.c  



door_src/skynet_module.o: ${DOOR_DEPS}  \
	door_src/skynet_module.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}  \
	-o door_src/skynet_module.o  \
	door_src/skynet_module.c



door_src/skynet_monitor.o: ${DOOR_DEPS}  \
	door_src/skynet_monitor.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}  \
	-o door_src/skynet_monitor.o  \
	door_src/skynet_monitor.c

  
door_src/skynet_mq.o: ${DOOR_DEPS}  \
	door_src/skynet_mq.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}  \
	-o door_src/skynet_mq.o  \
	door_src/skynet_mq.c

door_src/skynet_server.o: ${DOOR_DEPS}  \
	door_src/skynet_server.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}  \
	-o door_src/skynet_server.o  \
	door_src/skynet_server.c




door_src/skynet_socket.o: ${DOOR_DEPS}  \
	door_src/skynet_socket.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}  \
	-o door_src/skynet_socket.o  \
	door_src/skynet_socket.c




door_src/skynet_timer.o: ${DOOR_DEPS}  \
	door_src/skynet_timer.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}  \
	-o door_src/skynet_timer.o  \
	door_src/skynet_timer.c



door_src/socket_server.o: ${DOOR_DEPS}  \
	door_src/socket_server.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}  \
	-o door_src/socket_server.o  \
	door_src/socket_server.c


door_src/skynet_error.o: ${DOOR_DEPS}  \
	door_src/skynet_error.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}  \
	-o door_src/skynet_error.o  \
	door_src/skynet_error.c


door_src/skynet_main.o: ${DOOR_DEPS}  \
	door_src/skynet_main.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}  \
	-o door_src/skynet_main.o  \
	door_src/skynet_main.c

 door_src/skynet_start.o: ${DOOR_DEPS}  \
	door_src/skynet_start.c
	${CC} -c ${CFLAGS} ${DOOR_INCS}  \
	-o door_src/skynet_start.o  \
	door_src/skynet_start.c


clean:
	rm -f $(shell find -name "*.o")  
	rm -f door_net 


