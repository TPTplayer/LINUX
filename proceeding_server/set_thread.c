#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>

#include "set_thread.h"
#include "set_socket.h"
#include "util.h"

#define QUE 10
#define BUF_LEN 512

pthread_mutex_t key_client_fd = PTHREAD_MUTEX_INITIALIZER;
extern int *client_fd;
extern int *client_fd_stat;

void *connect_polling(void *argument){
	int cnt = 0, retval = 0, dummy;
	poll_arg *arg = (poll_arg*)argument;
	
	if((retval = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &dummy)) == -1){
		error_handler(errno, "pthread_setcanceltype");
		pthread_exit(NULL);
	}

	while(1){
		retval = epoll_wait(arg->epfd, arg->evnt, arg->maxevnts, -1);
		
		for(cnt = 0; cnt < retval; cnt++){
			pthread_mutex_lock(&key_client_fd); //critical section(startpoint)
			
			if(client_fd_stat[cnt] == 0){
				if((client_fd[cnt] = accept(arg->server_fd, NULL, NULL)) == -1){
					cnt -= 1;
					continue;
				}
				else{
					client_fd_stat[cnt] = 1;
				}

			}
			else{
				continue;
			}

			pthread_mutex_unlock(&key_client_fd); //critical section(endpoint)
		}
	}
}

void *rt_polling(void *argument){
	int epfd = 0, retval = 0, cnt = 0, received_sock = 0, dummy;
	char *RnT_buf;
	poll_rt *arg = (poll_rt*)argument;
	struct epoll_event *ep_evnt;

	if((retval = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS)) == -1){
		error_handler(errno, "pthread_setcanceltype");
		pthread_exit(NULL);
	}

	if((ep_evnt = malloc(sizeof(struct epoll_event) * arg->que)) == NULL){
		error_handler(errno, "malloc");
		pthread_exit(NULL);
	}

	if((RnT_buf = (char*)malloc(sizeof(char) * BUF_LEN)) == NULL){
		error_handler(errno, "malloc");
		pthread_exit(NULL);
	}

	epfd = epoll_create1(0);
	if(epfd == -1){
		pthread_exit(NULL);
	}
	
	pthread_mutex_lock(&key_client_fd); //critical section(startpoint)
	
	for(cnt = 0; cnt < arg->que; cnt++){
		ep_evnt[cnt].data.fd = client_fd[cnt];
		ep_evnt[cnt].events = EPOLLIN | EPOLLET;
		
		if((epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd[cnt], ep_evnt)) == -1){
			error_handler(errno, "epoll_ctl");
			pthread_exit(NULL);
		}
	}
	
	pthread_mutex_unlock(&key_client_fd); //critical section(endpoint)

	while(1){
		pthread_mutex_lock(&key_client_fd); //critical section(startpoint)
		
		if((retval = epoll_wait(epfd, ep_evnt, arg->que, -1)) == -1){
			error_handler(errno, "epoll_wait");
		}

		for(cnt = 0; cnt < retval; cnt++){
			if((recv(ep_evnt[cnt].data.fd, RnT_buf, BUF_LEN, 0)) == -1){
				error_handler(errno, "recv");
				pthread_exit(NULL);
			}
			
			received_sock = ep_evnt[cnt].data.fd;

			for(int i = 0; i < arg->que; i++){
				if(client_fd_stat[i] == 1 && client_fd[i] != received_sock){ //accepted client
					if((send(client_fd[i], RnT_buf, BUF_LEN, 0)) == -1){
						error_handler(errno, "send");
						pthread_exit(NULL);
					}
				}
				else{
					continue;
				}
			}
		}
		
		pthread_mutex_unlock(&key_client_fd); //critical section(endpoint)
	}
}

void *check_hangup(void *argument){
	int epfd = 0, cnt = 0, closed_client = 0, retval = 0, dummy;
	struct epoll_event *ep_evnt = malloc(sizeof(struct epoll_event) * QUE);
	
	if((retval = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS)) == -1){
		error_handler(errno, "pthread_setcanceltype");
		pthread_exit(NULL);
	}

	if(ep_evnt == NULL){
		error_handler(errno, "malloc");
		pthread_exit(NULL);
	}

	if((epfd = epoll_create1(0)) == -1){
		error_handler(errno, "epoll_create1");
		pthread_exit(NULL);
	}
	
	pthread_mutex_lock(&key_client_fd); //critical section(startpoint)

	for(int i = 0; i < QUE; i++){
		ep_evnt[i].data.fd = client_fd[i];
		ep_evnt[i].events = EPOLLHUP;
	}
	
	pthread_mutex_unlock(&key_client_fd); //critical section(endpoint)
	
	for(int i = 0; i < QUE; i++){
		if((epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd[i], ep_evnt)) == -1){
			error_handler(errno, "epoll_ctl");
			pthread_exit(NULL);
		}
	}

	while(1){
		if((cnt = epoll_wait(epfd, ep_evnt, QUE, -1)) == -1){
			error_handler(errno, "epoll_wait");
			pthread_exit(NULL);
		}
		
		pthread_mutex_lock(&key_client_fd); //critical section(startpoint)
		for(int i = 0; i < cnt; i++){
			closed_client = ep_evnt[i].data.fd;
			
			for(int j = 0; j < QUE; j++){
				if(closed_client == client_fd[j]){
					close(client_fd[j]);
					client_fd[j] = 0;
					client_fd_stat[j] = 0;
				}
			}
		}
		pthread_mutex_unlock(&key_client_fd); //critical section(endpoint)
	}
}
