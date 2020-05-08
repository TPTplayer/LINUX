#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>

#include "util.h"
#include "parallel.h"

int set_parallel(float *src_img, float *src_tmp, float *dst, int img_sx, int img_sy, int tmp_sx, int tmp_sy, int n_proc){
	int dst_sx = img_sx - tmp_sx + 1, dst_sy = img_sy - tmp_sy + 1;
	int dst_size = dst_sx * dst_sy;
	int pxl_per_thread = dst_size / n_proc, w = dst_size % n_proc;
	int e_code = 0;

	pthread_t *threads;
	t_arg *t_args;

	threads = malloc(sizeof(pthread_t) * n_proc);
	t_args = malloc(sizeof(t_arg) * n_proc);

	if(threads == NULL || t_args == NULL){
		UNIX_error_handler(errno, "malloc");
		return -1;
	}
	
	printf("%dpxl/thread\n\n", pxl_per_thread);

	for(int cnt = 0; cnt < n_proc; cnt++){
		t_args[cnt].src_img = src_img;
		t_args[cnt].src_tmp = src_tmp;
		t_args[cnt].dst = dst;
		t_args[cnt].img_sx = img_sx;
		t_args[cnt].tmp_sx = tmp_sx;
		t_args[cnt].tmp_sy = tmp_sy;
		t_args[cnt].dst_sx = dst_sx;

		t_args[cnt].start_dst_x = (cnt * pxl_per_thread) % dst_sx;
		t_args[cnt].start_dst_y = (cnt * pxl_per_thread) / dst_sx;

		if(cnt < n_proc - 1){
			t_args[cnt].end_dst_x = ((pxl_per_thread + (cnt * pxl_per_thread) - 1) % dst_sx) + 1;
			t_args[cnt].end_dst_y = (pxl_per_thread + (cnt * pxl_per_thread)) / dst_sx;
		}
		else if(cnt == n_proc - 1){
			t_args[cnt].end_dst_x = (((pxl_per_thread + w) + ((cnt * pxl_per_thread) - 1)) % dst_sx) + 1;
			t_args[cnt].end_dst_y = ((pxl_per_thread + w) + (cnt * pxl_per_thread)) / dst_sx;
		}
		
		e_code = pthread_create(&(threads[cnt]), NULL, parallel_ssd, (void *) (&(t_args[cnt])));
		if(e_code != 0){
			UNIX_error_handler(e_code, "pthread_create");
			return -1;
		}
	}

	for(int cnt = 0; cnt < n_proc; cnt++){
		e_code = pthread_join(threads[cnt], NULL);
		if(e_code != 0){
			UNIX_error_handler(e_code, "pthread_join");
			return -1;
		}
	}
	
	free(threads);
	free(t_args);

	return 0;
}

void * parallel_ssd(void *arg){
	float *src_img = (*(t_arg*)(arg)).src_img;
	float *src_tmp = (*(t_arg*)(arg)).src_tmp;
	float *dst = (*(t_arg*)(arg)).dst;
	int img_sx = (*(t_arg*)(arg)).img_sx;
	int tmp_sx = (*(t_arg*)(arg)).tmp_sx, tmp_sy = (*(t_arg*)(arg)).tmp_sy;
	int dst_sx = (*(t_arg*)(arg)).dst_sx;
	int sx = (*(t_arg*)(arg)).start_dst_x, sy = (*(t_arg*)(arg)).start_dst_y;
	int ex = (*(t_arg*)(arg)).end_dst_x, ey = (*(t_arg*)(arg)).end_dst_y;

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
