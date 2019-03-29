#define BUF_LEN 10

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void error_handler(int err);

int main(int argc, char **argv){
	int fd, err_num, close_return = 0;
	ssize_t nr, wr;
	char word[BUF_LEN], *write_buf;

	if(argc == 1){
		printf("Usage: <Filename For Create> <Filename For Read>\n");
		return 0;
	}
	

	fd = creat(argv[1], S_IRWXU);

	if(fd == -1){
		err_num = errno;
		error_handler(err_num);
		exit(-1);
	}	
	
	puts("Create Success");

	fd = open(argv[2], O_RDWR);
	
	if(fd == -1){
		err_num = errno;
		error_handler(err_num);
		exit(-1);
	}

	puts("open success");
	
	write_buf = (char*)malloc(BUF_LEN);
	gets(write_buf);
	write_buf[strlen(write_buf)+1] = '\0';

	wr = write(fd, write_buf, strlen(write_buf)+1);
	if(wr == -1){
		err_num = errno;
		error_handler(err_num);
		exit(-1);
	}

	puts("write success");

	while(1){
		nr = read(fd, word, BUF_LEN-1);
		
		if(nr == -1){
			err_num = errno;
			error_handler(err_num);
			exit(-1);
		}
		
		//printf("\ntest : %ld\n", nr); //value check

		if(nr == 0){
			break;
		}
		
		word[nr] = '\0';
		printf("%s", word);
	}

	puts("read success");
	/*
	for(int i = 0; i < 9; i++){
		printf("%x ", word[i]);
	}
	*/

	printf("fd: %d\n", fd);
	close_return = close(fd);
	
	if(close_return == -1){
		err_num = errno;
		error_handler(err_num);
		exit(-1);
	}
	printf("close: %d\n", close_return);

	return 0;
}

void error_handler(int err){
	char *errstr;
	errstr = strerror(err);

	while(1){
		putchar(*(errstr++));
		
		if(*errstr == '\0'){
			putchar('\n');
			break;
		}	
	}

}
