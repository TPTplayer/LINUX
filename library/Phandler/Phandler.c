#include "Phandler.h"
#include <Ehandler.h>

int fork_s(pid_t *prcs_arr, int prcs_num){
	int cnt = 0, parent = 0;
	
	parent = getpid();

	for(cnt = 0; cnt < prcs_num; cnt++){
		if(getpid() == parent){
			prcs_arr[cnt] = fork();
			
			if(getpid() != parent){
				prcs_arr[cnt] = getpid();	
			}

			if(prcs_arr[cnt] == -1){
				return errno;
			}
		}
	}

	return 0;
}
