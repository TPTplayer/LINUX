#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

#include "thread_func.h"

extern void error_handler(int err_num, char *func); //main.c의 에러 핸들러 사용
pthread_mutex_t API_Mutex = PTHREAD_MUTEX_INITIALIZER; //뮤텍스 초기화
struct TCB TCB_array[MAX]; //TCB 배열 생성

void tt_thread_register(int period, int thread_id){ //time triggered thread 등록함수(API의 대부분 제공됨)
	int res = 0;

	pthread_mutex_lock(&API_Mutex);
	TCB_array[thread_id].period = period; 
	TCB_array[thread_id].thread_id = thread_id;
	TCB_array[thread_id].time_left_to_invoke = period; 

	res = pthread_cond_init(&TCB_array[thread_id].pcond, NULL);
	if(res != 0){
		error_handler(res, "pthread_cond_init");
		exit(1);
	}

	pthread_mutex_unlock(&API_Mutex);
}

int tt_thread_wait_invocation(int thread_id){ //time triggered thread 대기(API의 일부 제공됨)
	int res = 0;

	pthread_mutex_lock(&API_Mutex);
	res = pthread_cond_wait(&TCB_array[thread_id].pcond, &API_Mutex); //time triggered thread 중지
	if(res != 0){
		error_handler(errno, "pthread_cond_wait");
		exit(1);
	}

	pthread_mutex_unlock(&API_Mutex);
	return 1; //명시적으로 1 반환
}

void *timer_signal_handler(void *NumOfThread){ //타이머(주요 소스)
	int num_of_thread = *(int*)NumOfThread, i = 0, res = 0; 
	long int tmp = 0, t_ref = 0, t_comp = 0, capture = 0, capture_ref = 0;
	int flag_t_ref_read = 1;

	struct timeval tv;

	while(1){
		while(1){
			if(flag_t_ref_read == 1){ //재진입 방지
				res = gettimeofday(&tv, NULL); //기준시간 얻기
				if(res == -1){
					error_handler(errno, "gettimeofday");
					exit(1);
				}	

				t_ref = tv.tv_usec; 
				flag_t_ref_read = 0; //재진입 방지
			}
			
			res = gettimeofday(&tv, NULL); //wait시간 구하기
			if(res == -1){
				error_handler(errno, "gettimeofday");
				exit(1);
			}
			t_comp = tv.tv_usec + capture; //capture: 보정값 

			if(t_comp > t_ref){ //overflow가 발생하지 않는 동안 바로 전의 시간차 저장
				tmp = t_comp - t_ref;
			}
			else if((t_comp - t_ref) < 0){ //overflow가 발생하여 t_comp가 0이 되고, 이에 따라 t_ref(10ms마다 읽음) 보다 값이 작아짐
				capture = tmp; //overflow가 발생하기 바로 전의 값을 보정값으로 대입
				flag_t_ref_read = 1; //재진입이 가능하게 만들어 t_ref의 값 갱신
				continue; //t_ref갱신
			}


			if((t_comp - t_ref) >= TIMELIMIT){ //TIMELIMIT default: 10ms (선언: thread_func.h)
				flag_t_ref_read = 1; //10ms경과시 재진입 가능하게 설정
				capture = 0; //보정값 초기화
				
				break; 
			}
		}
		
		pthread_mutex_lock(&API_Mutex);
		for(i = 0; i < num_of_thread; i++){ 
			TCB_array[i].time_left_to_invoke -= TIMELIMIT/1000; //적용까지의 남은 시간 갱신
			if(TCB_array[i].time_left_to_invoke <= 0){ //남은 시간이 없을경우
				TCB_array[i].time_left_to_invoke = TCB_array[i].period; //남은 시간을 초기값(period)로 되돌림
				res = pthread_cond_signal(&TCB_array[i].pcond); //signal을 보내 while에서 대기중인 thread를 깨운다
				if(res != 0){
					error_handler(errno, "pthread_cond_signal");
					exit(1);
				}
			}
		}
		pthread_mutex_unlock(&API_Mutex);
	}
}

void *timer_triggered_thread(void *arg){ //time triggered thread생성 시 실행되는 함수
	struct TCB T_info = *(struct TCB*)arg;
	struct timeval tv; 
	struct tm *time;

	tt_thread_register(T_info.period, T_info.thread_id); //등록
	while(tt_thread_wait_invocation(T_info.thread_id)){ //함수가 반환되면 loop안의 처리가 실행된다. 처리가 끝날 시 다시 wait.
		gettimeofday(&tv, NULL); //현재시간 구하기
		time = localtime(&tv.tv_sec); //가독성 증가
		printf("thread id: %d -> current time: %04d-%02d-%02d %02d:%02d:%02d.%02ld\n", T_info.thread_id, time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec, tv.tv_usec/10000);
	}
}

