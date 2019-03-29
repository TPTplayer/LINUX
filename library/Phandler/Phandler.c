#include "Phandler.h"

int fork_s(pid_t *prcs_arr, int prcs_num){
	int cnt = 0, parent = 0;
	
	parent = getpid();

	for(cnt = 0; cnt < prcs_num; cnt++){
		if(getpid() == parent){
			prcs_arr[cnt] = fork();

			if(prcs_arr[cnt] == -1){
				return errno;
			}
		}
	}

	return 0;
}

int create_daemon(){
	pid_t prcs = 0, parent = getpid(), session = 0;
	
	if(parent == getpid()){ //protect
		prcs = fork();

		if(prcs == -1){
			error_handler(errno, "fork");
			return -1;
		}
	}

	if(prcs == 0){
		session = setsid();
		
		if(session == -1){
			error_handler(errno, "fork");
			exit(-1);
		}

		prcs = getpid();
	}

	return prcs;
}
