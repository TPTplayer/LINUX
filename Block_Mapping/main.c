#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "blkmap.h"

FILE *devicefp;

/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);

int main(int argc, char *argv[])
{
	char *blockbuf;
    char sectorbuf[SECTOR_SIZE];
	int lsn, i;
	
	char **buf = (char**)malloc(sizeof(char*) * BLOCKS_PER_DEVICE);
	for(i = 0; i < BLOCKS_PER_DEVICE; i++){
		buf[i] = (char*)malloc(sizeof(char) * SECTOR_SIZE);
		sprintf(buf[i], "flandre scarlet");
	}

    devicefp = fopen("flashmemory", "w+b");

	if(devicefp == NULL)
	{
		printf("file open error\n");
		exit(1);
	}
	   
    blockbuf = (char *)malloc(BLOCK_SIZE);
	memset(blockbuf, 0xFF, BLOCK_SIZE);

	for(i = 0; i < BLOCKS_PER_DEVICE; i++)
	{
		fwrite(blockbuf, BLOCK_SIZE, 1, devicefp);
	}

	free(blockbuf);
	ftl_open(); 

	for(i = 0; i < BLOCKS_PER_DEVICE; i++){
		ftl_write(i, buf[i]);
	}

	for(i = 0; i < BLOCKS_PER_DEVICE; i++){
		lsn = i;

		ftl_read(i, sectorbuf);
		puts(sectorbuf);
	}

	fclose(devicefp);

	return 0;
}
