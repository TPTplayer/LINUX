typedef struct _thread_args{
	float *src_img;
	float *src_tmp;
	float *dst;
	int img_sx;
	int tmp_sx;
	int tmp_sy;
	int dst_sx;

	int start_dst_x;
	int start_dst_y;
	int end_dst_x;
	int end_dst_y;
}t_arg;

int set_parallel(float *src_img, float *src_tmp, float *dst, int img_sx, int img_sy, int tmp_sx, int tmp_sy, int n_proc);

void * parallel_ssd(void *arg);
