#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "gd.h"
#include "generator.h"

int main(int argc, char **argv){
	int res = 0, format = 0;
	int coordinate_x = 0, coordinate_y = 0, sx = 0, sy = 0;	

	if(argc != 4){
		fprintf(stderr, "Usage: %s <image> <template> <format>\n<format>: -jpeg or -png\n", argv[0]);
		return 0;
	}
	
	printf("start coordinate x: ");
	scanf("%d", &coordinate_x);

	printf("start coordinate y: ");
	scanf("%d", &coordinate_y);

	printf("template size x(pixel): ");
	scanf("%d", &sx);

	printf("template size y(pixel): ");
	scanf("%d", &sy);
	
	if(!strcmp(argv[3], "-jpeg")){
		format = JPEG;
	}
	else if(!strcmp(argv[3], "-png")){
		format = PNG;
	}
	else{
		fprintf(stderr, "Not supported\n");
		return 0;
	}

	res = template_generator(argv[1], argv[2], format, sx, sy, coordinate_x, coordinate_y);
	if(res == -1){
		exit(EXIT_FAILURE);
	}
	
	puts("complete");

	return 0;
}
