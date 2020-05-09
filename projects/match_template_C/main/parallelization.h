#include "gd.h"

typedef struct _thread_args{
//	gdImagePtr image;
	float *src_img;
	float *src_tmp;
	float *dst;
	
	int img_sx;
//	int img_sy;

	int tmp_sx;
	int tmp_sy;
	
	int dst_sx;
//	int dst_sy;

	int start_x;
	int start_y;

	int end_x;
	int end_y;
}t_arg;

//int setting_parallelized_getpxl_from_gdImage_grayscale(float *data, gdImagePtr image, int n_proc);
int setting_parallelized_ssd(float *src_img, float *src_tmp, float *dst, int img_sx, int img_sy, int tmp_sx, int tmp_sy, int n_proc);

