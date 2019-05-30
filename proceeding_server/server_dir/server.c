#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "set_socket.h"

#define QUE 10
#define TIMEOUT 5
#define BUF_LEN 1024

int que_size = QUE;

int main(int argc, char **argv){
	int server_fd = -1, client_fd = -1, port = 0, retval = 0, flag = 0, client_log[1024], client_cnt = 0;
	int read_byte = 0, write_byte = 0;
	char buf[BUF_LEN];
	struct sockaddr_in *server_addr;

	if(argc != 2){	
		fprintf(stderr, "Usage: ./<filename> <port>\n");
		return 1;
	}
	
	memset(client_log, -1, QUE);
	memset(buf, 0, BUF_LEN);

	port = atoi(argv[1]);
	retval = set_socket(&server_fd, server_addr, port);

	if(retval!= 0){
		errno = retval;
		error_handler(errno, "set_socket");
		exit(1);
	}

	while(1){
		client_fd = accept(server_fd, NULL, NULL);
		if(client_fd == -1){
			continue;
		}
			
		flag = fcntl(client_fd, F_GETFL, 0);
		fcntl(client_fd, F_SETFL, flag | O_NONBLOCK); //socket set: nonblock mode | flag
		client_log[client_cnt] = client_fd;

		read_byte = read(client_fd, buf, BUF_LEN);
		if(read_byte == -1){
			if(errno == EAGAIN){
				continue;	
			}
			else if(errno == EINTR){
				continue;
			}
			else{
				error_handler(errno, "read");
				exit(1);
			}
		}
		else{
			for(int i = 0; i < client_cnt; i++){
				if(client_log[i] == client_fd){
					continue;
				}
				if(client_log[i] == -2){ //hangup handle
					continue;
				}

				write_byte = write(client_log[i], buf, BUF_LEN);
				if(write_byte == -1){
					if(errno == EAGAIN){ 
						i -= 1;
						continue;
					}
					else if(errno == EINTR){
						i -= 1;
						continue;
					}
					else{
						client_log[i] = -2; //hangup
					}
				}
			}
		}
	}
}
