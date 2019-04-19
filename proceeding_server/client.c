#include <stdlio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>

#include "util.h"
#include "set_client_socket.h"

#define BUF_LEN 512

int main(int argc, char **argv){
	int port = 0, client_fd = 0, retval = 0;
	char *buf_receive, *buf_transmit;
	pthread_t thread_r, thread_t;

	if(argc != 2){
		fprintf(stderr, "Usage: ./<filename> <server port number>\n");
		return 1;
	}

	port = atoi(argv[1]);
	
	buf_receive = (char*)malloc(sizeof(char) * BUF_LEN);
	buf_transmit = (char*)malloc(sizeof(char) * BUF_LEN);

	if(buf_receive == NULL || buf_transmit == NULL){
		error_handler(errno, "malloc");
		exit(1);
	}
	
	if((pthread_create(thread_r, NULL, )))

	return 0;
}
