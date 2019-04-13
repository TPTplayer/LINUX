#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>

#include "set_thread.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *polling(void *argument);

int background_polling(int epfd, struct epoll_event *ep_evnt, int limit){
	arg_struct *argm;
	pthread_t thread;
	int retval = 0;
	
	argm = (argm*)malloc(sizeof(argm));
	if(argm == NULL){
		printf("malloc: ");
		return errno;
	}
	
	argm->epfd = epfd;
	argm->limit = limit;
	argm->ep_evnt = ep_evnt;

	retval = pthread_create(&thread, NULL, polling, (void*)argm);
	if(retval == -1){
		printf("pthread_create: ");
		errno = retval;
		return errno;
	}

	retval = pthread_join(thread, NULL);
	if(retval == -1){
		printf("pthread_join: ");
		return errno;
	}

	return 1;
}

void *polling(void *argument){
	int num_evnt = 0, cnt = 0;
	arg_struct *argm = (arg_struct*)argument;

	while(1){
		if((num_evnt = epoll_wait(argm->epfd, argm->ep_evnt, argm->limit, -1)) == -1){
			fprintf(stderr, "epoll_wait: %s\n", strerror(errno));
			pthread_exit(NULL);
		}
		
		for(cnt = 0; cnt < num_evnt; cnt++){
			
		}
	}
}

