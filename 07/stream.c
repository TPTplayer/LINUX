#include <stdio.h>
#include <Lsystem.h>

#define LEN 1024

typedef struct info{
	char name[LEN];
	int temp;
}info;

int main(void){
	FILE *stream;
	info pasv, recv; 
	
	int err_num, return_value = 0;
	size_t write_element, read_element, num_elmnt = 1;
	
	strcpy(pasv.name, "IRFP250");
	pasv.temp = 175;

	stream = fopen("data", "w");
	if(stream == NULL){
		err_num = errno;
		error_handler(err_num);
		
		exit(-1);
	}

	write_element = fwrite(&pasv, sizeof(info), num_elmnt, stream);
	if(write_element < num_elmnt){
		err_num = errno;
		error_handler(err_num);

		exit(-1);
	}

	return_value = fclose(stream);
	if(return_value == EOF){
		err_num = errno;
		error_handler(err_num);

		exit(-1);
	}
	
	stream = fopen("data", "r");
	if(stream == NULL){
		err_num = errno;
		error_handler(errno);

		exit(-1);
	}

	read_element = fread(&recv, sizeof(info), num_elmnt, stream);
	if(read_element < num_elmnt){
		err_num = errno;
		error_handler(errno);

		exit(-1);
	}
	
	return_value = fclose(stream);
	if(return_value == EOF){
		err_num = errno;
		error_handler(errno);

		exit(-1);
	}

	printf("name: %s\ntemperature: %d\n", recv.name, recv.temp);
	return 0;
}
