#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

#include "match_template.h"

int main(int argc, char **argv){
	int res = 0, format = 0, n_proc = 1;
	int coordinate[2] = {0, 0};	
	char n_proc_buf[3] = {0, };
	double t = 0.0;
	struct timeval start, end;

	if(argc < 5 || argc > 6){
		fprintf(stderr, "Usage: %s <image> <template> <result> <format> <-j<n>>\n<format>: -jpeg or -png\n<-j<n>>: n is number of threads to parallelize (optional)\n", argv[0]);
		exit(EXIT_SUCCESS);
	}	
	
	if(!strcmp(argv[4], "-jpeg")){
		format = JPEG;
	}
	else if(!strcmp(argv[4], "-png")){
		format = PNG;
	}
	else{
		fprintf(stderr, "Not supported\n");
		exit(EXIT_SUCCESS);
	}

	if(argc == 6){
		if(strlen(argv[5]) == 3 && (argv[5][2] != '0' || argv[5][2] != '1')){
			n_proc_buf[0] = argv[5][2];
			n_proc = atoi(n_proc_buf);
		}
		else if(strlen(argv[5]) == 4 && argv[5][2] != '0'){
			n_proc_buf[0] = argv[5][2];
			n_proc_buf[1] = argv[5][3];
			n_proc = atoi(n_proc_buf);
		}
		else{
			fprintf(stderr, "Warning: This thread count is not supported. Set to default: 1\n");
			n_proc = 1;
		}
	}
	
	printf("processing thread: %d\n", n_proc);

	gettimeofday(&start, NULL);
	res = match_template(argv[1], argv[2], argv[3], format, coordinate, n_proc);
	if(res == -1){
		exit(EXIT_FAILURE);
	}
	gettimeofday(&end, NULL);
	
	t = ((double)end.tv_sec + ((double)end.tv_usec / 1000000.0)) - ((double)start.tv_sec + ((double)start.tv_usec / 1000000.0));

	printf("matched (x, y): (%d, %d)\n", coordinate[1], coordinate[0]);
	printf("processing time: %.6lfs\n", t);

	puts("complete");
	exit(EXIT_SUCCESS);	
}
