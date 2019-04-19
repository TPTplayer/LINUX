#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <errno.h>

#include "set_socket.h"
#include "set_thread.h"
#include "util.h"

#define QUE 10

int client_fd[QUE];
int client_fd_stat[QUE];

int main(int argc, char **argv){
	int server_fd = 0, epfd = 0, retval = 0;
	struct epoll_event *ep_evnt;
	poll_arg *arg;
	poll_rt *rt_arg;

	pthread_t thread_poll, thread_rt, thread_hangup;

	if(argc != 2){
		fprintf(stderr, "Usage: ./<filename> <port number>\n");
		return 1;
	}

	if((set_socket(&server_fd, atoi(argv[1]))) != 0){
		error_handler(errno, "set_socket");
		exit(1);
	}
	
	if((ep_evnt = malloc(sizeof(struct epoll_event) * QUE)) == NULL){
		error_handler(errno, "malloc");
		exit(1);
	}
	
	if((arg = malloc(sizeof(poll_arg))) == NULL){
		error_handler(errno, "malloc");
		exit(1);
	}

	if((rt_arg = malloc(sizeof(poll_rt))) == NULL){
		error_handler(errno, "malloc");
		exit(1);
	}

	if((set_polling(&epfd, &server_fd, ep_evnt)) != 0){
		error_handler(errno, "set_polling");
		exit(1);
	}
	
	arg->epfd = epfd;
	arg->maxevnts = QUE;
	arg->server_fd = server_fd;
	
	rt_arg->client_fd = client_fd;
	rt_arg->que = QUE;

	if((retval = pthread_create(&thread_poll, NULL, connect_polling, arg)) != 0){
		error_handler(errno, "pthread_create");
		exit(1);
	}
	
	if((retval = pthread_create(&thread_rt, NULL, rt_polling, rt_arg)) != 0){
		error_handler(errno, "pthread_create");
		exit(1);
	}
	
	if((retval = pthread_create(&thread_hangup, NULL, check_hangup, NULL)) != 0){
		error_handler(errno, "pthread_create");
		exit(1);
	}

	if((retval = pthread_join(thread_poll, NULL)) != 0){
		error_handler(retval, "pthread_join");
		exit(1);
	}

	if((retval = pthread_join(thread_rt, NULL)) != 0){
		error_handler(retval, "pthread_join");
		exit(1);
	}
	
	if((retval = pthread_join(thread_hangup, NULL)) != 0){
		error_handler(retval, "pthread_join");
		exit(1);
	}

	return 0;
}
