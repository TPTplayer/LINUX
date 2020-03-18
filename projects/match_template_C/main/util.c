#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "util.h"

void UNIX_error_handler(int err, char *func){
	fprintf(stderr, "%s: %s\n", func, strerror(err));
	return;
}

