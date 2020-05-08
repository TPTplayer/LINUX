#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>

#include "util.h"

#define EVENT_BUF_LEN 256

int main(int argc, char **argv){
	int res = -1, cnt = 0, len = 0, flag_ignored = 0;
	int file_descriptor = 0, watch_descriptor = 0;
	char *path, *file_log, *file_err, *timebuf;
   	char event_buf[EVENT_BUF_LEN] __attribute__((aligned(4)));
	struct stat info_buf;
	struct inotify_event *event;

	if(argc == 2 && strcmp(argv[1], "-config") == 0){
		res = set_config();
		if(res < 0){
			error_handler(-res, "set_config");
			exit(EXIT_FAILURE);
		}
	}
	else if((argc == 2 && strcmp(argv[1], "-config")) || (argc > 2)){
		fprintf(stderr, "Read the ReadMe\n");
		exit(EXIT_FAILURE);
	}
		
	path = malloc(sizeof(char) * PATH_LEN);
	file_log = malloc(sizeof(char) * PATH_LEN);
	file_err = malloc(sizeof(char) * PATH_LEN);
	timebuf = malloc(sizeof(char) * TIME_LEN);
	if(path == NULL || file_log == NULL || file_err == NULL || timebuf == NULL){
		error_handler(errno, "malloc");
		exit(EXIT_FAILURE);
	}
	
	memset(path, 0, PATH_LEN);
	memset(file_log, 0, PATH_LEN);
	memset(file_err, 0, PATH_LEN);
	memset(timebuf, 0, TIME_LEN);
	
	res = read_config(path, file_log, file_err);
	if(res < 0){
		if(-res == ENOENT){
			fprintf(stderr, "Read the ReadMe\n");
			exit(EXIT_FAILURE);
		}

		error_handler(-res, "read_config");
		exit(EXIT_FAILURE);
	}

	res = stat(path, &info_buf);
	if(res == -1){
		error_handler(errno, "stat");
		exit(EXIT_FAILURE);
	}
	
	res = stdio_err_redirection(file_log, file_err);
	if(res < 0){
		error_handler(-res, "stdio_err_redirection");
		exit(EXIT_FAILURE);
	}

	file_descriptor = inotify_init1(O_NONBLOCK);
	if(file_descriptor == -1){
		error_handler(errno, "inotify_init1");
		exit(EXIT_FAILURE);
	}
	
	watch_descriptor = inotify_add_watch(file_descriptor, path, IN_ALL_EVENTS);
	if(watch_descriptor == -1){
		error_handler(errno, "inotify_add_watch");
		exit(EXIT_FAILURE);
	}
	
	while(1){
		len = read(file_descriptor, event_buf, EVENT_BUF_LEN);
		if(len == -1){
			if(errno == EAGAIN){
				continue;
			}
			else{
				error_handler(errno, "read");
				exit(EXIT_FAILURE);
			
			}
		}
		
		for(cnt = 0; cnt < len; cnt += sizeof(struct inotify_event) + event -> len){
			res = get_current_time(timebuf);
			if(res < 0){
				error_handler(-res, "get_current_time");
			}
			fprintf(stdout, "TIME: %s\n", timebuf);
		
			event = (struct inotify_event *) &event_buf[cnt];	
			if((event -> mask & IN_ACCESS) && (event -> len != 0)){
				fprintf(stdout, "Read: %s\n", event -> name);
			}
			else if((event -> mask & IN_ACCESS) && (event -> len == 0)){
				fprintf(stdout, "Read: %s\n", path);
			}

			if((event -> mask & IN_MODIFY) && (event -> len != 0)){
				fprintf(stdout, "Write: %s\n", event -> name);
			}
			else if((event -> mask & IN_MODIFY) && (event -> len == 0)){
				fprintf(stdout, "Write: %s\n", path);
			}

			if((event -> mask & IN_ATTRIB) && (event -> len != 0)){
				fprintf(stdout, "Metadata changed: %s\n", event -> name);
			}
			else if((event -> mask & IN_ATTRIB) && (event -> len == 0)){
				fprintf(stdout, "Metadata changed: %s\n", path);
			}

			if((event -> mask & IN_CLOSE_WRITE) && (event -> len != 0)){
				fprintf(stdout, "File closed - Write: %s\n", event -> name);
			}
			else if((event -> mask & IN_CLOSE_WRITE) && (event -> len == 0)){
				fprintf(stdout, "File closed - Write: %s\n", path);
			}

			if((event -> mask & IN_CLOSE_NOWRITE) && (event -> len != 0)){
				fprintf(stdout, "File closed: %s\n", event -> name);
			}
			else if((event -> mask & IN_CLOSE_NOWRITE) && (event -> len == 0)){
				fprintf(stdout, "File closed: %s\n", path);
			}

			if((event -> mask & IN_OPEN) && (event -> len != 0)){
				fprintf(stdout, "File opened: %s\n", event -> name);
			}
			else if((event -> mask & IN_OPEN) && (event -> len == 0)){
				fprintf(stdout, "File opened: %s\n", path);
			}

			if((event -> mask & IN_MOVED_TO) && (event -> len != 0)){
				fprintf(stdout, "Moved to: %s\n", event -> name);
			}
			else if(event -> mask & IN_MOVED_FROM){
				fprintf(stdout, "Moved from: %s\n", event -> name);
			}
			else if(event -> mask & IN_CREATE){
				fprintf(stdout, "File created: %s\n", event -> name);
			}
			else if(event -> mask & IN_DELETE){
				fprintf(stdout, "File deleted: %s\n", event -> name);
			}
			else if(event -> mask & IN_DELETE_SELF){
				fprintf(stdout, "Watched object %s has been DELETED\n", path);
			}
			else if(event -> mask & IN_MOVE_SELF){
				fprintf(stdout, "Watched object %s has been MOVED\n", path);
			}

			if(event -> mask & IN_IGNORED){
				fprintf(stderr, "TIME: %s\n", timebuf);
				fprintf(stderr, "Watcher has been REMOVED\n");
				flag_ignored = 1;
				break;
			}
			if(event -> mask & IN_Q_OVERFLOW){
				fprintf(stderr, "TIME: %s\n", timebuf);
				fprintf(stderr, "QUEUE OVERFLOW\n");
			}
			if(event -> mask & IN_UNMOUNT){
				fprintf(stderr, "TIME: %s\n", timebuf);
				fprintf(stderr, "UNMOUNTED DEVICE\n");
			}
			putchar('\n');
		}

		if(flag_ignored == 1){
			free(path);
			free(timebuf);
			
			exit(EXIT_FAILURE);
			break;
		}
	}

	return 0;
}
