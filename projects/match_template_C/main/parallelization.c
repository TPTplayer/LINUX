#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>

#include "gd.h"
#include "util.h"
#include "parallelization.h"
#include "preprocess.h"
#include "ssd.h"

/*
int setting_parallelized_getpxl_from_gdImage_grayscale(float *data, gdImagePtr image, int n_proc){
	int sx = (*image).sx, sy = (*image).sy, size = sx * sy;
	int pxl_per_thread = size / n_proc, w = size % n_proc;
	int e_code = 0;

	pthread_t *threads;
	t_arg *t_args;

	threads = malloc(sizeof(pthread_t) * n_proc);
	t_args = malloc(sizeof(t_arg) * n_proc);

	if(threads == NULL || t_args == NULL){
		UNIX_error_handler(errno, "malloc");
		return -1;
	}

	for(int cnt = 0; cnt < n_proc; cnt++){
		t_args[cnt].image = image;
		t_args[cnt].dst = data;
		t_args[cnt].dst_sx = sx;
		
		t_args[cnt].start_x = (cnt * pxl_per_thread) % sx;
		t_args[cnt].start_y = (cnt * pxl_per_thread) / sx;
		
		if(cnt < n_proc - 1){
			t_args[cnt].end_x = ((pxl_per_thread + (cnt * pxl_per_thread) - 1) % sx) + 1;
			t_args[cnt].end_y = (pxl_per_thread + (cnt * pxl_per_thread)) / sx;
		}
		else if(cnt == n_proc - 1){
			t_args[cnt].end_x = 0;
			t_args[cnt].end_y = (pxl_per_thread + (cnt * pxl_per_thread) + w) / sx;
		}

		e_code = pthread_create(&(threads[cnt]), NULL, parallel_getpxl_from_gdImage_grayscale, (void*) (&(t_args[cnt])));
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
*/

int setting_parallelized_ssd(float *src_img, float *src_tmp, float *dst, int img_sx, int img_sy, int tmp_sx, int tmp_sy, int n_proc){
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

		t_args[cnt].start_x = (cnt * pxl_per_thread) % dst_sx;
		t_args[cnt].start_y = (cnt * pxl_per_thread) / dst_sx;

		if(cnt < n_proc - 1){
			t_args[cnt].end_x = ((pxl_per_thread + (cnt * pxl_per_thread) - 1) % dst_sx) + 1;
			t_args[cnt].end_y = (pxl_per_thread + (cnt * pxl_per_thread)) / dst_sx;
		}
		else if(cnt == n_proc - 1){
			t_args[cnt].end_x = 0;
			t_args[cnt].end_y = (pxl_per_thread + (cnt * pxl_per_thread) + w) / dst_sx;
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


