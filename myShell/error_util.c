#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "error_util.h"

void error_handler(int err_num, char *func){
	char buf[ERR_BUF];
	memset(buf, 0, ERR_BUF);
	
	strerror_r(err_num, buf, ERR_BUF);
	fprintf(stderr, "%s: %s\n", func, buf);
}
