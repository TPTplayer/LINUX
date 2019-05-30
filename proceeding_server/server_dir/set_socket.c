#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "set_socket.h"

#define SIZE sizeof(struct sockaddr_in)

extern int que_size;

int set_socket(int *server_fd, struct sockaddr_in *server_addr, int port){
	if((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		return errno;
	}

	(*server_addr).sin_family = AF_INET;
	(*server_addr).sin_port = port;
	(*server_addr).sin_addr.s_addr = INADDR_ANY;

	if((bind(*server_fd, (struct sockaddr*)server_addr, SIZE)) == -1){
		return errno;
	}

	if((listen(*server_fd, que_size)) == -1){
		return errno;
	}

	return 0;
}

