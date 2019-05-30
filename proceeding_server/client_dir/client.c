#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include "util.h"
#include "set_socket.h"

#define BUF_LEN 1024

int socket_flag = 0, client_fd = -1;
void receive(void *arg);

int main(int argc, char **argv){
	int write_byte = 0, retval = 0;
	char *id, write_buf[BUF_LEN];
	pthread_t receive_thread;

	if(argc != 3){
		fprintf(stderr, "Usage: %s <port> <ID>\n", argv[0]);
		return 1;
	}
	
	port = atoi(argv[1]);
	memset(write_buf, 0, BUF_LEN);

	id = malloc(sizeof(char) * 20);
	if(id == NULL){
		error_handler(errno, "malloc");
		exit(1);
	}
	
	while(1){
		if(socket_flag == 0){
			continue;
		}	

		fgets(write_buf, BUF_LEN, stdin);
		write_byte = write(client_fd, write_buf, BUF_LEN);
		if(write_byte == -1){
			error_handler(errno, "write");
		}

		
	}
}

void *receive(void *port){
	int read_byte = 0, retval = 0;
	char read_buf[BUF_LEN];
	struct sockaddr_in *server_addr;
	
	memset(read_buf, 0, BUF_LEN);
	retval = set_socket(&client_fd, server_addr, *port);
	if(retval != 0){
		error_handler(errno, "set_socket");
		pthread_exit(NULL);
	}
		
	socket_flag = 1;

	while(1){
		
	}
}
