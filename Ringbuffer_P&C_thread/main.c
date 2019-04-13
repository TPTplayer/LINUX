#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RingBuffer.h"
#include "util.h"

#define SLEEPTIME 100 //ms

pthread_mutex_t key = PTHREAD_MUTEX_INITIALIZER;
pthread_t tid_producer, tid_consumer;

void* producer_thread(void *buf_set);
void* consumer_thread(void *buf_set);

int main(int argc, char **argv){ 
	pthread_t producer, consumer;
	int *argm, num_argm = 0;

	RingBuf *buf;
	Ringbuf_set buf_set;
	
	if(argc == 1){
		fprintf(stderr, "Usage: ./<Filename> <Argumnet1> <Argument2>...\n");
		return 1;
	}

	buf = (RingBuf*)malloc(sizeof(RingBuf));
	argm = (int*)malloc(sizeof(int)*argc);
	num_argm = argc-1;
	
	for(int i = 0; i < num_argm; i++){
		argm[i] = atoi(argv[i+1]);
	}

	buf_set.buf = buf;
	buf_set.num_argm = num_argm;
	buf_set.argm = argm;

	pthread_create(&producer, NULL, producer_thread, (void*)&buf_set);
	pthread_create(&consumer, NULL, consumer_thread, (void*)&buf_set);
	
	tid_producer = producer;
	tid_consumer = consumer;

	pthread_join(producer, NULL);
	pthread_join(consumer, NULL);

	return 0;
}

void* producer_thread(void *buf_set){
	struct timeval tv;
	Ringbuf_set buf_s = *(Ringbuf_set*)buf_set;

	int retval = 0, cnt = 0, dummy;

	tv.tv_sec = 0;
	tv.tv_usec = 1000*SLEEPTIME;

	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &dummy);

	while(1){
		pthread_mutex_lock(&key); /*critical section start*/
		
		retval = IsFull(buf_s.buf);
		if(retval == 0){
			if(cnt == buf_s.num_argm){
				pthread_mutex_unlock(&key);
				break;
			}

			retval = BufInput(buf_s.buf, buf_s.argm[cnt]);
			if(retval == -1){
				pthread_mutex_unlock(&key);
				break;
			}

			cnt += 1;
		}
		else if(retval == -1){
			pthread_mutex_unlock(&key);
			break;
		}
		
		pthread_mutex_unlock(&key); /*critical section end*/
		select(0, NULL, NULL, NULL, &tv);
	}

	pthread_cancel(tid_consumer);
	pthread_exit(NULL);
}

void* consumer_thread(void *buf_set){
	Ringbuf_set buf_s = *(Ringbuf_set*)buf_set;
	int retval = 0, dummy;
	
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &dummy);

	while(1){
		pthread_mutex_lock(&key); /*critical section start*/
		
		retval = IsEmpty(buf_s.buf);
		if(!retval){
			retval = BufOutput(buf_s.buf);
			if(retval == -1){
				pthread_mutex_unlock(&key);
				break;
			}

			fprintf(stdout, "%d\n", retval);
		}
			
		pthread_mutex_unlock(&key);/*critical section end*/
	}
	
	pthread_cancel(tid_producer);
	pthread_exit(NULL);
}
