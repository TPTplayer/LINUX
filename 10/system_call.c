#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

#include <Ehandler.h>
#include <Phandler.h>
#include <fileIO.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_LEN 1024

int main(void){
	pid_t d_pid = 0, parent = getpid();
	int fd = 0, cnt = 0;
	char buf[BUF_LEN];

	if(parent == getpid()){
		puts("parent: create");
		d_pid = create_daemon();
	}

	if(d_pid == getpid()){
		fd = open("daemon process.txt", O_CREAT | O_WRONLY | O_APPEND, S_IRWXU);
		puts("daemon: open success");

		if(fd == -1){
			error_handler(errno, "open");
			exit(-1);
		}

		while(1){
			sprintf(buf, "daemon %d\n", cnt);
			wrFile(fd, buf, BUF_LEN, strlen(buf));

			cnt++;

			sleep(2);
		}
	}
	
	if(parent == getpid()){
		exit(0);
	}

	return 0;
}
