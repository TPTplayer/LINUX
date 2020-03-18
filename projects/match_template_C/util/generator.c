#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "gd.h"
#include "util.h"
#include "generator.h"

int template_generator(char *image, char *result, int format, int dst_sx, int dst_sy, int coordinate_x, int coordinate_y){
	FILE *image_file, *result_file;
	gdImagePtr im_src, im_dst;
	int color = 0;

	image_file = fopen(image, "rb");
	result_file = fopen(result, "wb");
	if(image_file == NULL || result_file == NULL){
		UNIX_error_handler(errno, "fopen");
		return -1;
	}
	
	if(format == JPEG){
		im_src = gdImageCreateFromJpeg(image_file);
	}
	else if(format == PNG){
		im_src = gdImageCreateFromPng(image_file);
	}
	else{
		return -1;
	}
	
	if(coordinate_x + dst_sx > (*im_src).sx || coordinate_y + dst_sy > (*im_src).sy){
		return -1;
	}
	
	im_dst = gdImageCreateTrueColor(dst_sx, dst_sy);
	
	if(im_src == NULL || im_dst == NULL){
		return -1;
	}

	for(int y = 0; y < dst_sy; y++){
		for(int x = 0; x < dst_sx; x++){
			color = gdImageGetTrueColorPixel(im_src, coordinate_x + x, coordinate_y + y);
			gdImageSetPixel(im_dst, x, y, color);
		}
	}

	if(format == JPEG){
		gdImageJpeg(im_dst, result_file, -1);
	}
	else{
		gdImagePng(im_dst, result_file);
	}

	gdImageDestroy(im_src);
	gdImageDestroy(im_dst);
	
	fclose(image_file);
	fclose(result_file);

	return 0;
}


