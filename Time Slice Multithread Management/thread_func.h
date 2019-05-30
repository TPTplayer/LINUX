#define MAX 10
#define TIMELIMIT 10000 //단위: us

/*us -> ms의 환산: 1ms = 1000us*/
/*TIMELIMIT default: 10000us = 10ms*/

struct TCB{
	int thread_id;
	int period;
	int time_left_to_invoke;
	pthread_cond_t pcond;
};

void tt_thread_register(int period, int thread_id);
int tt_thread_wait_invocation(int thread_id);
void *timer_signal_handler(void *arg);
void *timer_triggered_thread(void *arg);
