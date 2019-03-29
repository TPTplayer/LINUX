#include "fileIO.h"

int putsFile(int fd, char *buf, size_t buf_len){
	ssize_t read_byte;

	while(1){
		read_byte = read(fd, buf, buf_len-1);

		if(read_byte == -1){ //error
			close(fd);
			return errno;
		}
		if(read_byte == 0){ //EOF
			break;
		}

		buf[read_byte] = '\0';
		printf("%s", buf);

		memset(buf, 0, strlen(buf)+1); //buffer flush
	}

	return 0;
}

int rdFile(int fd, void *buf, size_t buf_len){
	ssize_t read_byte;

	while(buf_len != 0 && (read_byte = read(fd, buf, buf_len))){
		if(read_byte == -1){
			if(errno == EINTR){ //cause: system interrupt
				continue;
			}

			return errno; //error
		}

		buf_len -= read_byte;
		buf += read_byte;
	}

	return 0;
}

int wrFile(int fd, void *buf, size_t buf_len, size_t data_len){
	ssize_t write_byte;

	if(data_len > buf_len){
		fprintf(stderr, "WARNING: Exceeds length\n");
		return 1;
	}

	while(data_len != 0 && (write_byte = write(fd, buf, data_len))){
		if(write_byte == -1){
			if(errno == EINTR){
				continue;
			}

			return errno;
		}

		data_len -= write_byte;
		buf += write_byte; 
	}

	return 0;
}

int readdir_all(DIR *directory, struct dirent *D_entry){
	while((D_entry = readdir(directory)) != NULL){
		printf("%s\n", D_entry->d_name);
	}

	if(errno != 0){ //error
		return errno;
	}

	return 0;
}

int readv_abs(int fd, struct iovec *iov, int cnt, void **buf, int *data_len){
	int i = 0, return_value = 0;

	for(i = 0; i < cnt; i++){
		iov[i].iov_base = buf[i];
		iov[i].iov_len = data_len[i];
	}	

	return_value = readv(fd, iov, cnt);
	if(return_value == -1){
		return errno;
	}

	return 0;
}

int writev_abs(int fd, struct iovec *iov, int cnt, void **buf, int *data_len){
	int i = 0, return_value = 0;

	for(i = 0; i < cnt; i++){
		iov[i].iov_base = buf[i];
		iov[i].iov_len = data_len[i];
	}

	return_value = writev(fd, iov, cnt);
	if(return_value == -1){
		return errno;
	}

	return 0;
}
