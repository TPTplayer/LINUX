#include <unistd.h>
#include <sys/uio.h>
#include <dirent.h>
#include <errno.h>

#include <stdio.h>
#include <string.h>

int putsFile(int, char*, size_t);
int rdFile(int, void*, size_t);
int wrFile(int, void*, size_t, size_t);
int readdir_all(DIR*, struct dirent*);
int readv_abs(int, struct iovec*, int, void**, int*);
int writev_abs(int, struct iovec*, int, void**, int *);
