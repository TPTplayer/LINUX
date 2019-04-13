#include "set_socket.h"
#include "set_thread.h"

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

int server_ready(int *server_fd, struct sockaddr_in *addr, int option, int size, int q_size){
	*server_fd = socket(AF_INET, SOCK_STREAM, option);
	if(*server_fd == -1){
		printf("socket: ");
		return errno;
	}

	if((bind(*server_fd, (struct sockaddr*)addr, size)) == -1){
		printf("bind: ");
		return errno;
	}

	if((listen(*server_fd, q_size)) == -1){
		printf("listen: ");
		return errno;
	}

	return 0;
}

int server_polling(int *server_fd, struct epoll_event *ep_evnt, int limit){
	int epfd = 0, num_evnt = 0, retval = 0;
	
	epfd = epoll_create1(0);
	if(epfd == -1){
		printf("epoll_create1: ");
		return errno;
	}

	if(epoll_ctl(epfd, EPOLL_CTL_ADD, *server_fd, ep_evnt) == -1){
		close(epfd);
		printf("epoll_ctl: ");
		return errno;
	}
	
	
}


