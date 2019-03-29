#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Lsystem.h>

int main(int argc, char **argv){
	int check_access = 0;
	int read = 0, write = 0, excute = 0;
	//flag: 0 - file is not exist
	//flag: 1 - No authority on this process
	//flag: 2 - read available
	//flag: 3 - write available
	//flag: 4 - excution available
	//flag: 5 - RW available
	//flag: 6 - RX available
	//flag: 7 - WX available
	//flag: 8 - RWX available

	if(argc == 1){
		puts("Usage: <File path for check>");
		return 1;
	}
	

	check_access = access(argv[1], F_OK);
	if(check_access != 0){
		puts("file is not exist");
		return 0;
	}
	
	check_access = access(argv[1], R_OK);
	if(check_access == 0){
		read = 1;
	}

	check_access = access(argv[1], W_OK);
	if(check_access == 0){
		write = 1;
	}

	check_access = access(argv[1], X_OK);
	if(check_access == 0){
		excute = 1;
	}

	if(read == 1){
		putchar('R');
	}
	if(write == 1){
		putchar('W');
	}
	if(excute == 1){
		putchar('X');
	}
	putchar('\n');

	return 0;
}
