#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>

#include "thread_func.h"

void error_handler(int err_num, char *func);

int main(void){
	int tt_thread_num = 0, *period, res = 0, cnt = 0;
	pthread_t *thread_arr, timer;
	struct TCB *T_info;

	period = malloc(sizeof(int) * MAX);
	memset(period, 0, MAX);
	
	thread_arr = malloc(sizeof(pthread_t) * MAX);
	T_info = malloc(sizeof(struct TCB) * MAX); //time triggered thread 등록에 필요한 정보

	while(1){
		printf("Number of threads(MAX: 10): ");
		scanf("%d", &tt_thread_num);

		if(tt_thread_num > 10){
			puts("invaild value");
			getchar();
	
			system("clear");
			continue;
		}
		break;
	}
	
	for(cnt = 0; cnt < tt_thread_num; cnt++){
		printf("Thread %d - period(ms, Require at least 1ms): ", cnt);
		scanf("%d", &(period[cnt]));
	}
	
	for(cnt = 0; cnt < tt_thread_num; cnt++){
		T_info[cnt].thread_id = cnt;
		T_info[cnt].period = period[cnt];

		res = pthread_create(&(thread_arr[cnt]), NULL, timer_triggered_thread, &(T_info[cnt])); //time triggered thread 생성
		if(res == -1){
			error_handler(errno, "pthread_create");
			exit(1);
		}
	}
	
	res = pthread_create(&timer, NULL, timer_signal_handler, &tt_thread_num); //타이머 생성
	if(res == -1){
		error_handler(errno, "pthread_create");
		exit(1);
	}

	while(1){
		pause(); //main thread 종료 방지
	}	

	return 0;
}

void error_handler(int err_num, char *func){ //에러 핸들러
	fprintf(stderr, "%s: %s\n", func, strerror(err_num));
}
