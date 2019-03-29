#include "Ehandler.h"

void error_handler(int err_num, char *func_name){
	char *strerr = strerror(err_num);

	fprintf(stderr, "%s:%s\n", func_name, strerr);
}
