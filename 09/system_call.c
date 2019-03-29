#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <Ehandler.h>
#include <Phandler.h>

#include <string.h>
#include <stdlib.h>

#define P_NUM 3

int main(int argc, char **argv){
	pid_t pid[P_NUM], demon_prcs, pid_parents;
	int cnt = 0, status, return_value = 0;
	
	pid_parents = getpid();
	
	return_value = fork_s(pid, P_NUM);
	if(return_value != 0){
		error_handler(return_value, "fork_s");
	}

	if(getpid() != pid_parents){ //in child process
		printf("child process: %d\nsession: %d\n", getpid(), getsid(0));
		exit(0);
	}

	if(getpid() == pid_parents){
		for(cnt = 0; cnt < P_NUM; cnt++){
			printf("process %d exit\n", waitpid(pid[cnt], &status, 0));
		}
	}
	
	/*
	demon_prcs = fork(); //create new process
	if(getpid() != pid_parents){
		printf("old session: %d\n", getsid(0));
		printf("new session: %d\n", setsid());

		while(1){
			fprintf(stdout,"%d: I'm Warking!\n", demon_prcs);
			sleep(500);
		}
	}
	*/

/*
	if(getpid() == pid_parents){
		waitpid(demon_prcs, &status, 0);
	}
*/
	printf("exit parents process: %d\n", getpid());
	return 0;
}
