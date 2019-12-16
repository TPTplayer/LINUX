#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "util.h"

void error_handler(int err, char *func){
	fprintf(stderr, "%s: %s\n", func, strerror(err));
	return;
}

int stdio_err_redirection(char *file_log, char *file_err){
	int log = -1, err = -1;
	int res = -1;

	log = open(file_log, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
	if(log == -1){
		return -errno;
	}

	err = open(file_err, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
	if(err == -1){
		return -errno;
	}

	res = dup2(log, 1);
	if(res == -1){
		return -errno;
	}

	res = dup2(err, 2);
	if(res == -1){
		return -errno;
	}

	return 0;
}

int get_current_time(char *buf){
	int res = -1;
	struct timeval tv;
	struct tm *tm;

	res = gettimeofday(&tv, NULL);
	if(res == -1){
		return -errno;
	}

	tm = localtime(&tv.tv_sec);
	
	res = sprintf(buf, "%d.%d.%d - %d:%d:%d", tm -> tm_year + 1900, tm -> tm_mon + 1, tm -> tm_mday, tm -> tm_hour, tm -> tm_min, tm -> tm_sec);
	
	return 0;
}

int set_config(void){
	int res = -1;
	int config_fd = -1;
	char *path, *file_log, *file_err;
	
	path = malloc(sizeof(char) * PATH_LEN);
	file_log = malloc(sizeof(char) * PATH_LEN);
	file_err = malloc(sizeof(char) * PATH_LEN);

	if(path == NULL || file_log == NULL || file_err == NULL){
		return -errno;
	}
	memset(path, 0, PATH_LEN);
	memset(file_log, 0, PATH_LEN);
	memset(file_err, 0, PATH_LEN);

	printf("path: ");
	fgets(path, PATH_LEN, stdin);
	
	putchar('\n');
	
	printf("filename (log): ");
	fgets(file_log, PATH_LEN, stdin);

	putchar('\n');

	printf("filename (error log): ");
	fgets(file_err, PATH_LEN, stdin);

	config_fd = open(".config", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
	if(config_fd == -1){
		return -errno;
	}

	res = write(config_fd, path, strlen(path));
	if(res == -1){
		return -errno;
	}

	res = write(config_fd, file_log, strlen(file_log));
	if(res == -1){
		return -errno;
	}

	res = write(config_fd, file_err, strlen(file_err));
	if(res == -1){
		return -errno;
	}

	close(config_fd);
	free(path);
	free(file_log);
	free(file_err);

	return 0;
}

int read_config(char *path, char *file_log, char *file_err){
	int res = -1, cnt = 0, offset = 0;
	int config_fd = -1;
	char *str_config;	
	
	str_config = malloc(sizeof(char) * PATH_LEN * 3);
	if(str_config == NULL){
		return -errno;
	}
	memset(str_config, 0, PATH_LEN * 3);

	config_fd = open(".config", O_RDONLY);
	if(config_fd == -1){
		return -errno;
	}	

	res = read(config_fd, str_config, PATH_LEN * 3);
	if(res == -1){
		return -errno;
	}
	
	for(cnt = 0; cnt < PATH_LEN; cnt++){
		if(str_config[cnt] == '\n'){
			offset = cnt + 1;
			break;
		}

		path[cnt] = str_config[cnt];
	}
	
	for(cnt = 0; cnt < PATH_LEN; cnt++){
		if(str_config[cnt + offset] == '\n'){
			offset += cnt + 1;
			break;
		}

		file_log[cnt] = str_config[cnt + offset];
	}
	
	for(cnt = 0; cnt < PATH_LEN; cnt++){
		if(str_config[cnt + offset] == '\n' || str_config[cnt + offset] == 0){
			break;
		}

		file_err[cnt] = str_config[cnt + offset];
	}
	
	free(str_config);
	close(config_fd);
	return 0;
}
