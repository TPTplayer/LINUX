#include <math.h>
#include "ssd.h"
#include "parallelization.h"

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

void * parallel_ssd(void *arg){
	float *src_img = (*(t_arg*)(arg)).src_img;
	float *src_tmp = (*(t_arg*)(arg)).src_tmp;
	float *dst = (*(t_arg*)(arg)).dst;
	int img_sx = (*(t_arg*)(arg)).img_sx;
	int tmp_sx = (*(t_arg*)(arg)).tmp_sx, tmp_sy = (*(t_arg*)(arg)).tmp_sy;
	int dst_sx = (*(t_arg*)(arg)).dst_sx;
	int sx = (*(t_arg*)(arg)).start_x, sy = (*(t_arg*)(arg)).start_y;
	int ex = (*(t_arg*)(arg)).end_x, ey = (*(t_arg*)(arg)).end_y;

	int end = (ey * dst_sx) + ex;
	
	float diff = 0.0F, c = 0.0F;

	for(int idx = 0; idx < end; idx = (sy * dst_sx) + sx){
		diff = 0.0F;
		for(int ty = 0; ty < tmp_sy; ty++){
			for(int tx = 0; tx < tmp_sx; tx++){
				c = src_tmp[(ty * tmp_sx) + tx] - src_img[((ty + sy) * img_sx) + (tx + sx)];
				diff += (float)(pow(c, 2.0F));
			}
		}
		
		diff = 1.0F / sqrt(diff);
		dst[(sy * img_sx) + sx] = diff;

		sx += 1;
		if(sx == img_sx){
			sy += 1;
			sx = 0;
		}
	}

	return;
}
