#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#define SIZE sizeof(struct sockaddr_in)
#define QUE 10

int set_socket(int *server_fd, int port_num){
	struct sockaddr_in server_addr;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = port_num;
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		return errno;
	}

	if((bind(*server_fd, (struct sockaddr*)&server_addr, SIZE)) == -1){
		return errno;
	}

	if((listen(*server_fd, QUE)) == -1){
		return errno;
	}

	return 0;
}

int set_polling(int *epfd, int *server_fd, struct epoll_event *ep_evnt){
	*epfd = epoll_create1(0);
	if(*epfd == -1){
		return errno;
	}

	if((epoll_ctl(*epfd, EPOLL_CTL_ADD, *server_fd, ep_evnt)) == -1){
		return errno;
	}

	return 0;
}
