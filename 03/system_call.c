#define BUF_LEN 20

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Lsystem.h>

int main(int argc, char **argv){
	int fd, err_num, return_value = 0;
	char buf[BUF_LEN];

	if(argc < 2){
		puts("Usage: <Filename for open> <cut length(intager)>");
		return 0;
	}

	fd = open(argv[1], O_RDWR, S_IRWXU);
	if(fd == -1){
		err_num = errno;
		error_handler(err_num);
		exit(-1);
	}

	printf("%s(%d): open success\n", argv[1], fd);
	printf("FILE %s >\n", argv[1]);
	
	return_value = rdFile(fd, buf, (size_t)BUF_LEN);
	if(return_value != 0){
		err_num = return_value;
		error_handler(err_num);
		exit(-1);
	}
		
	printf("%s(%d): read success\n", argv[1], fd);

	return_value = ftruncate(fd, (off_t)atoi(argv[2]));
	if(return_value == -1){
		err_num = errno;
		error_handler(err_num);
		exit(-1);
	}
	
	return_value = lseek(fd, (off_t)0, SEEK_SET);
	if(return_value == -1){
		err_num = errno;
		error_handler(err_num);
		exit(-1);
	}

	printf("FILE %s >\n", argv[1]);

	return_value = rdFile(fd, buf, (size_t)BUF_LEN);
	if(return_value != 0){
		err_num = return_value;
		error_handler(err_num);
		exit(-1);
	}

	printf("%s(%d): read success\n", argv[1], fd);
	
	return_value = close(fd);
	if(return_value == -1){
		err_num = errno;
		error_handler(err_num);
		exit(-1);
	}

	return 0;
}
