#include <stdio.h>
#include <string.h>
#include <errno.h>

void error_handler(int err_num, char *func){
	fprintf(stderr, "%s: %s\n", func, strerror(err_num));
}
