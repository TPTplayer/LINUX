#include <math.h>
#include "gd.h"
#include "parallelization.h"
#include "preprocess.h"

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
			data[(y * sx) + x] = (r * RED_W) + (g * GREEN_W) + (b * BLUE_W);
		}
	}

	return;
}

/*
void * parallel_getpxl_from_gdImage_grayscale(void *arg){
	gdImagePtr image = (*(t_arg*)(arg)).image;
	float *data = (*(t_arg*)(arg)).dst;
	int data_sx = (*(t_arg*)(arg)).dst_sx;
	int sx = (*(t_arg*)(arg)).start_x, sy = (*(t_arg*)(arg)).start_y;
	int ex = (*(t_arg*)(arg)).end_x, ey = (*(t_arg*)(arg)).end_y;

	int end = (ey * data_sx) + ex;

	float r = 0.0F, g = 0.0F, b = 0.0F;
	int color = 0;

	for(int idx = 0; idx < end; idx = (sy * data_sx) + sx){
		color = gdImageGetPixel(image, sx, sy);	

		r = gdImageRed(image, color);
		g = gdImageGreen(image, color);
		b = gdImageBlue(image, color);

		data[(sy * data_sx) + sx] = (r * RED_W) + (g * GREEN_W) + (b * BLUE_W);
		
		sx += 1;
		if(sx == data_sx){
			sy += 1;
			sx = 0;
		}	
	}

	return;
}
*/

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


