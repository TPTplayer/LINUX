#include <stdio.h>
#include "gd.h"

#define JPEG 0
#define PNG 1

int match_template(char *path_image, char *path_template, char *path_result, int format, int *coordinate, int n_proc);
void getpxl_from_gdImage_grayscale(float *data, gdImagePtr image);
float calc_mean(float *data, int sx, int sy);
float calc_std(float *data, int sx, int sy, float mean);
void normalization(float *data, int sx, int sy);
int find_matched_coordinate(float *data, int sx, int sy, int wx, int wy);
void ssd(float *src_img, float *src_tmp, float *dst, int img_sy, int img_sx, int tmp_sx, int tmp_sy);
int match_template_ssd(FILE *image_file, FILE *template_file, char *name_result, int format, int *coordinate, int n_proc);
