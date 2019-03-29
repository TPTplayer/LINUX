#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Ehandler.h>

int fork_s(pid_t*, int);
int create_daemon();
