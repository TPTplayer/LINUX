#include "gd.h"

#define RED_W 0.299F
#define GREEN_W 0.587F
#define BLUE_W 0.114F

void getpxl_from_gdImage_grayscale(float *data, gdImagePtr image);
//void * parallel_getpxl_from_gdImage_grayscale(void *arg);

float calc_mean(float *data, int sx, int sy);
float calc_std(float *data, int sx, int sy, float mean);
void normalization(float *data, int sx, int sy);
