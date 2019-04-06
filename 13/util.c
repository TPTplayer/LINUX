#include <stdio.h>
#include "util.h"

void errH(char *efunc,char *emsg) {
	fprintf(stderr, "%s: %s\n", efunc, emsg);
}