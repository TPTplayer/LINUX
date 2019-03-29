#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Lsystem.h>

#define BUF_LEN 1024

int main(void){
	int err_num, return_value = 0, fd;
	char buf[1024] = {'\0',};

	fd = open("test", O_RDONLY);
	if(fd == -1){
		err_num = errno;
		error_handler(err_num);
		exit(-1);
	}
	
	return_value = rdFile(fd, buf, (size_t)BUF_LEN);
	if(return_value != 0){
		error_handler(return_value);
		exit(-1);
	}

	buf[strlen(buf)+1] = '\0';

	printf("read: %s", buf);
	return 0;
}









