#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
	int fd1, fd2;
	char descripe[100], *pt;
	
	//printf("debug: %d\n", argc);

	if(argc == 1){
		printf("usage: <File name for Create> <File name for Open>\n");
		exit(-1);
	}

	//fd1 = open(argv[1], O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG);
	fd1 = creat(argv[1], S_IRWXU | S_IRWXG);

	if(fd1 == -1){
		pt = strerror(errno);

		for(int i = 0; i < 100; i++){
			descripe[i] = *(pt++);

			if(descripe[i] == '\0'){
				break;
			}
		}
		
		printf("fd1_error: %s\n", descripe);
	}
	else{
		printf("fd1: %d\n", fd1);
	}

	fd2 = open(argv[2], O_RDONLY);

	if(fd2 == -1){
		pt = strerror(errno);

		for(int i = 0; i < 100; i++){
			descripe[i] = *(pt++);

			if(descripe[i] == '\0'){
				break;
			}
		}

		printf("fd2_error: %s\n", descripe);
	}
	else{
		printf("fd2: %d\n", fd2);
	}


	return 0;
}
