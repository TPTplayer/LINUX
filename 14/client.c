#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include <Ehandler.h>

#define PORT 5000
#define BUF_LEN 512
#define SIZE sizeof(struct sockaddr_in)

int main(void){
	int socket_fd = -1, retval = 0;
	char receive_buf[BUF_LEN], transmit_buf[BUF_LEN];
	struct sockaddr_in sock_addres;

	sock_addres.sin_family = AF_INET;
	sock_addres.sin_port = PORT;
	sock_addres.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	while(1){

		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if(socket_fd == -1){
			error_handler(errno, "socket");
			sleep(1);

			continue;
		}


		retval = connect(socket_fd, (struct sockaddr*)&sock_addres, SIZE);
		if(retval == -1){
			error_handler(errno, "connect");
			sleep(1);
			
			continue;
		}

		memset(transmit_buf, 0, BUF_LEN);
		memset(receive_buf, 0, BUF_LEN);
		
		fgets(transmit_buf, BUF_LEN, stdin);
		
		retval = send(socket_fd, transmit_buf, BUF_LEN, 0);
		if(retval == -1){
			error_handler(errno, "send");
			continue;
		}
		
		retval = recv(socket_fd, receive_buf, BUF_LEN, 0);
		if(retval == -1){
			error_handler(errno, "recv");
			continue;
		}

		fprintf(stdout, "server: %s", receive_buf);
	}	

	return 0;
}
