#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "util.h"

void error_handler(int err_num, char *func){
	fprintf(stderr, "%s: %s\n", func, strerror(err_num));
}
