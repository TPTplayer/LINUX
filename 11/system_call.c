#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <signal.h>

#include <Ehandler.h>
#include <fileIO.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void){
	int return_value = 0;
	pid_t pid = 4000;

	return_value = daemon(0, 0);
	
	while(1){
		if(kill(SIGKILL, pid) == -1){
			if(kill(SIGINT, pid) == -1){
				continue;	
			}
		}
	}
}
