#include "set_client_socket.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#define SIZE sizeof(struct sockaddr_in)

int set_socket(int *fd, int port){
	struct sockaddr_in server_addr = {AF_INET, port};
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	if((*fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		return errno;
	}

	if((connect(*fd, (struct sockaddr_in*)&server_addr, SIZE)) == -1){
		return errno;
	}

	return 0;
}

