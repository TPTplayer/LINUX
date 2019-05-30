#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uinstd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet.h>
#include <arpa/inet.h>
#include <errno.h>

#include "set_socket.h"

#define SIZE sizeof(struct sockaddr_in)

int set_socket(int *client_fd, struct sockaddr_in *server_addr, int port){
	if((*client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		return errno;
	}

	(*server_addr).sin_family = AF_INET;
	(*server_addr).sin_port = port;
	(*server_addr).sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(client_fd, (struct sockaddr*)server_addr, SIZE) == -1){
		return errno;
	}

	return 0;
}
