#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fileIO.h>
#include <Ehandler.h>

#define BUF_NUM 3
#define BUF_LEN 1024

int main(int argc, char **argv){
	int fd, err_num = 0, return_value = 0, cnt = 0, str_len[BUF_NUM];
	struct iovec iov[BUF_NUM], iov_read[BUF_NUM];

	char *buf[BUF_NUM]; // buf: [pointer][pointer][pointer]...(number of index: BUF_NUM)
	char *read_buf[BUF_NUM]; //read_buf: [pointer][pointer][pointer]...(number of index: BUF_NUM)

	if(argc < 2){
		puts("Usage: <Filename for Create & R/W>");
		return 1;
	}

	for(cnt = 0; cnt < BUF_NUM; cnt++){
		buf[cnt] = (char*)malloc(sizeof(char)*BUF_LEN);
	}

	fd = creat(argv[1], S_IRWXU);
	if(fd == -1){
		err_num = errno;
		error_handler(err_num);

		exit(-1);
	}

	puts("clear: open");
	
	for(cnt = 0; cnt < BUF_NUM; cnt++){
		fgets(buf[cnt], BUF_LEN, stdin);
		str_len[cnt] = strlen(buf[cnt])+1;

		printf("str_len[%d]: %d\n", cnt, str_len[cnt]);
	}

	return_value = writev_abs(fd, iov, BUF_NUM, buf, str_len);
	if(return_value != 0){
		err_num = errno;
		error_handler(err_num);

		exit(-1);
	}
	
	puts("clear: write");

	for(cnt = 0; cnt < BUF_NUM; cnt++){
		free(buf[cnt]);
		read_buf[cnt] = (char*)malloc(sizeof(char)*BUF_LEN);
	}
	
	return_value = close(fd);
	if(return_value == -1){
		err_num = errno;
		error_handler(err_num);

		exit(-1);
	}

	fd = open(argv[1], O_RDONLY);
	
	return_value = readv_abs(fd, iov_read, BUF_NUM, read_buf, str_len);
	if(return_value != 0){
		err_num = errno;
		error_handler(err_num);

		exit(-1);
	}
	
	puts("clear: read");

	for(cnt = 0; cnt < BUF_NUM; cnt++){
		printf("%s", read_buf[cnt]);
		free(read_buf[cnt]);
	}
	
	return_value = close(fd);
	if(return_value == -1){
		err_num = errno;
		error_handler(err_num);

		exit(-1);
	}

	return 0;
}
