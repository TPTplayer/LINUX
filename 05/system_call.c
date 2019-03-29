#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Lsystem.h>

int main(int argc, char **argv){
	int err_num = 0, return_value = 0;
	DIR *directory;
	struct dirent *D_entry;

	if(argc == 1){
		puts("Usage: <Directory Name for Open & Read>");
		return 1;
	}
	
	directory = opendir(argv[1]);
	if(directory == NULL){
		err_num = errno;
		error_handler(err_num);
		exit(-1);
	}
	
	return_value = readdir_all(directory, D_entry);
	if(return_value != 0){
		error_handler(return_value);
		exit(-1);
	}
	
	closedir(directory);

	return 0;
}

