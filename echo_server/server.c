#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>

#include <Ehandler.h>


#define PORT 5000
#define SIZEQ 5
#define SIZE sizeof(struct sockaddr_in)
#define BUF_LEN 512

int main(void){
	int socket_fd = -1, fd_connect = -1, retval = 0;
	char receive_buf[BUF_LEN], transmit_buf[BUF_LEN];
	struct sockaddr_in sock_addres;
	
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd == -1){
		error_handler(errno, "socket");
		exit(1);
	}

	sock_addres.sin_family = AF_INET;
	sock_addres.sin_port = PORT;
	sock_addres.sin_addr.s_addr = INADDR_ANY;

	retval = bind(socket_fd, (struct sockaddr*)&sock_addres, SIZE);
	if(retval == -1){
		close(socket_fd);
		error_handler(errno, "bind");
		exit(1);
	}

	retval = listen(socket_fd, SIZEQ);
	if(retval == -1){
		close(socket_fd);
		error_handler(errno, "listen");
		exit(1);
	}

	while(1){
		memset(receive_buf, 0, BUF_LEN);
		memset(transmit_buf, 0, BUF_LEN);

		fd_connect = accept(socket_fd, NULL, NULL);
		if(fd_connect == -1){
			error_handler(errno, "accept");
			continue;
		}
		

		retval = recv(fd_connect, receive_buf, BUF_LEN, 0);
		if(retval == -1){

			error_handler(errno, "recv");
			continue;
		}
		
		printf("server: received: %s", receive_buf);
		memcpy(transmit_buf, receive_buf, BUF_LEN);

		retval = send(fd_connect, transmit_buf, BUF_LEN, 0);
		if(retval == -1){
			error_handler(errno, "send");
			continue;
		}
		
		printf("server: transmit: %s", transmit_buf);

		close(fd_connect);
	}
	
	return 0;
}
