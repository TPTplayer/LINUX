#include "gd.h"

#define JPEG 0
#define PNG 1

int match_template(char *path_image, char *path_template, char *path_result, int format, int *coordinate, int n_proc);
int find_matched_coordinate(float *data, int sx, int sy, int wx, int wy);
int match_template_ssd(FILE *image_file, FILE *template_file, char *name_result, int format, int *coordinate, int n_proc);
