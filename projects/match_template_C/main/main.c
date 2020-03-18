#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "match_template.h"

int main(int argc, char **argv){
	int res = 0, format = 0;
	int coordinate[2] = {0, 0};	

	if(argc != 5){
		fprintf(stderr, "Usage: %s <image> <template> <result> <format>\n<format>: -jpeg or -png\n", argv[0]);
		return 0;
	}	
	
	if(!strcmp(argv[4], "-jpeg")){
		format = JPEG;
	}
	else if(!strcmp(argv[4], "-png")){
		format = PNG;
	}
	else{
		fprintf(stderr, "Not supported\n");
		return 0;
	}

	res = match_template(argv[1], argv[2], argv[3], format, coordinate);
	if(res == -1){
		exit(EXIT_FAILURE);
	}
	printf("matched: (%d, %d)\n", coordinate[1], coordinate[0]);

	puts("complete");
	return 0;	
}
