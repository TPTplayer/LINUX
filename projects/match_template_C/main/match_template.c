#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>

#include "util.h"
#include "gd.h"
#include "match_template.h"
#include "parallel.h"

int match_template(char *path_image, char *path_template, char *path_result, int format, int *coordinate, int n_proc){
	int res = 0;
	FILE *image, *template;
	
	image = fopen(path_image, "rb");
	template = fopen(path_template, "rb");
	
	if(image == NULL || template == NULL){
		UNIX_error_handler(errno, "fopen");
		return -1;
	}

	res = match_template_ssd(image, template, path_result, format, coordinate, n_proc);		
	if(res == -1){
		return -1;
	}

	fclose(image);
	fclose(template);

	return 0;
}

void getpxl_from_gdImage_grayscale(float *data, gdImagePtr image){
	int sx = (*image).sx, sy = (*image).sy;
	float r = 0.0F, g = 0.0F, b = 0.0F;
	int color = 0;

	for(int y = 0; y < sy; y++){
		for(int x = 0; x < sx; x++){
			color = gdImageGetPixel(image, x, y);
			r = gdImageRed(image, color);
			g = gdImageGreen(image, color);
			b = gdImageBlue(image, color);
			data[(y * sx) + x] = (r * 0.299F) + (g * 0.587F) + (b * 0.114F);
		}
	}

	return;
}

float calc_mean(float *data, int sx, int sy){
	float sum_n = 0.0F;
	int n = sx * sy;

	for(int idx = 0; idx < n; idx++){
		sum_n += data[idx];
	}
	return sum_n / (float)n;
}

float calc_std(float *data, int sx, int sy, float mean){
	float std = 0.0F;
	int n = sx * sy;

	for(int idx = 0; idx < n; idx++){
		std += (float)pow(data[idx] - mean, 2.0F);
	}
	return sqrt(std / (float)(n - 1));
}

void normalization(float *data, int sx, int sy){
	float std = 0.0F, mean = 0.0F;
	int n = sx * sy;

	mean = calc_mean(data, sx, sy);
	std = calc_std(data, sx, sy, mean);

	for(int idx = 0; idx < n; idx++){
		data[idx] = (data[idx] - mean) / std;
	}

	return;
}

void ssd(float *src_img, float *src_tmp, float *dst, int img_sx, int img_sy, int tmp_sx, int tmp_sy){
	int dst_sx = img_sx - tmp_sx + 1, dst_sy = img_sy - tmp_sy + 1;
	float diff = 0.0F, c = 0.0F;

	for(int iy = 0; iy < dst_sy; iy++){
		for(int ix = 0; ix < dst_sx; ix++){
			diff = 0.0F;
			for(int ty = 0; ty < tmp_sy; ty++){
				for(int tx = 0; tx < tmp_sx; tx++){
					c = src_tmp[(ty * tmp_sx) + tx] - src_img[((ty + iy) * img_sx) + (ix + tx)];
					diff += (float)(pow(c, 2.0F));
				}
			}

			diff = 1.0F / sqrt(diff);
			dst[(iy * img_sx) + ix] = diff;
		}
	}

	return;
}

int match_template_ssd(FILE *image_file, FILE *template_file, char *name_result, int format, int *coordinate, int n_proc){
	FILE *result_file;
	gdImagePtr image, template, result;
	float *pxl_image, *pxl_template, *pxl_result, max = 0.0F;
	int image_sx = 0, image_sy = 0, template_sx = 0, template_sy = 0;
	int mx = 0, my = 0, dst_sx = 0, dst_sy = 0;
	int res = 0;

	puts(" ");
	puts("image memory allocation");

	if(format == JPEG){
		image = gdImageCreateFromJpeg(image_file);
		template = gdImageCreateFromJpeg(template_file);
		if(image == NULL || template == NULL){
			return -1;
		}
	}
	else if(format == PNG){
		image = gdImageCreateFromPng(image_file);
		template = gdImageCreateFromPng(template_file);
		if(image == NULL || template == NULL){
			return -1;
		}
	}
	else{
		return -1;
	}

	image_sx = (*image).sx;
	image_sy = (*image).sy;
	template_sx = (*template).sx;
	template_sy = (*template).sy;
	
	puts(" ");
	printf("Number of pixels in the image: %dpxl\n", image_sx * image_sy);
	printf("\tx: %dpxl\n\ty: %dpxl\n", image_sx, image_sy);
	
	puts(" ");
	printf("Number of pixels in the template image: %dpxl\n", template_sx * template_sy);
	printf("\tx: %dpxl\n\ty: %dpxl\n", template_sx, template_sy);
	
	puts(" ");
	puts("data memory allocation");

	pxl_image = malloc(sizeof(float) * image_sx * image_sy);
	pxl_template = malloc(sizeof(float) * template_sx * template_sy);
	pxl_result = malloc(sizeof(float) * image_sx * image_sy);
	if(pxl_image == NULL || pxl_template == NULL || pxl_result == NULL){
		return -1;
	}
	
	puts("data preprocessing");

	getpxl_from_gdImage_grayscale(pxl_image, image);
	getpxl_from_gdImage_grayscale(pxl_template, template);
	
	normalization(pxl_image, image_sx, image_sy);
	normalization(pxl_template, template_sx, template_sy);
	
	puts("data processing (SSD)\n");
	
	if(n_proc == 1){
		ssd(pxl_image, pxl_template, pxl_result, image_sx, image_sy, template_sx, template_sy);
	}
	else{
		res = set_parallel(pxl_image, pxl_template, pxl_result, image_sx, image_sy, template_sx, template_sy, n_proc);
		if(res == -1){
			return -1;
		}		
	}

	for(int y = 0; y < image_sy; y++){
		for(int x = 0; x < image_sx; x++){
			if(max < pxl_result[(y * image_sx) + x]){
				max = pxl_result[(y * image_sx) + x];
				mx = x;
				my = y;
			}
		}
	}
	
	coordinate[0] = my;
	coordinate[1] = mx;

	dst_sx = image_sx - template_sx + 1;
	dst_sy = image_sy - template_sy + 1;

	result_file = fopen(name_result, "wb");
	if(result_file == NULL){
		UNIX_error_handler(errno, "fopen");
		return -1;
	}
	
	result = gdImageCreateTrueColor(dst_sx, dst_sy);
	if(result == NULL){
		return -1;
	}

	for(int y = 0; y < dst_sy; y++){
		for(int x = 0; x < dst_sx; x++){
			gdImageSetPixel(result, x, y, (int)(pxl_result[(y * image_sx) + x] * 0x1FFF));
		}
	}
	if(format == JPEG){
		gdImageJpeg(result, result_file, -1);
	}
	else{
		gdImagePng(result, result_file);
	}

	free(pxl_image);
	free(pxl_template);
	free(pxl_result);

	gdImageDestroy(image);
	gdImageDestroy(template);
	gdImageDestroy(result);
	fclose(result_file);

	return 0;
}

