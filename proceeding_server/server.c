#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "set_epoll.h"
#include "set_socket.h"

#define DEFAULT 0
#define PORT 5000
#define SIZE sizeof(struct sockaddr_in)
#define Q_SIZE 10

void error_handler(int err_num, char *func);

int main(void){
	int server_fd = 0;
	int retval = 0;
	struct sockaddr_in addr = {AF_INET, PORT, INADDR_ANY};
	struct epoll_event *ep_evnt;

	retval = server_ready(&server_fd, &addr, DEFAULT, SIZE);
	if(retval != 0){
		fprintf(stderr, "%s\n", strerror(retval));
		exit(1);
	}
	
	ep_evnt = malloc(sizeof(struct epoll_evnet)*Q_SIZE);
	if(ep_evnt == NULL){
		error_handler(errno, "malloc");
		close(server_fd);
		exit(1);
	}

	retval = server_polling(&server_fd, ep_evnt, Q_SIZE);
	if(retval != 0){
		fprintf(stderr, "%s\n", strerror(retval));
		
		free(ep_evnt);
		close(server_fd);
		
		exit(1);
	}
	else{
		close(server_fd);
		free(ep_evnt);

		return 0;
	}
}

void error_handler(int err_num, char *func){
	fprintf(stderr, "%s: %s\n", strerror(err_num), func);
}
