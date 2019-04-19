#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include "blkmap.h"

extern FILE *devicefp;
int block_table[DATABLKS_PER_DEVICE]; 
int free_block[1];

void ftl_open(){
	int cnt = 0, pbn = 0;
	int isSame = FALSE, isFirst = TRUE;
	
	srand(time(NULL));

	for(cnt = 0; cnt < DATABLKS_PER_DEVICE; cnt++){
		if(isFirst == TRUE){
			block_table[cnt] = (rand() % DATABLKS_PER_DEVICE);
			isFirst = FALSE;
		}
		else{
			pbn = (rand() % DATABLKS_PER_DEVICE);
			for(int i = 0; i < cnt; i++){
				if(block_table[i] == pbn){
					isSame = TRUE;
					break;
				}
			}

			if(isSame == TRUE){
				cnt -= 1;
				isSame = FALSE;
				continue;
			}
			else{
				block_table[cnt] = pbn;
				if((dd_erase(block_table[cnt])) == -1){
					fprintf(stderr, "error: dd_erase\n");
					exit(1);
				}
			}
		}
	}

	free_block[0] = BLOCKS_PER_DEVICE-1;
	if((dd_erase(BLOCKS_PER_DEVICE-1)) == -1){
		fprintf(stderr, "error: dd_erase\n");
		exit(1);
	}

	return;
}

void ftl_read(int lsn, char *sectorbuf){
	int lbn, pbn, psn, offset, retval;
	
	lbn = lsn / PAGES_PER_BLOCK; //get logical block number
	offset = lsn % PAGES_PER_BLOCK; //get offset
	pbn = block_table[lbn]; //get physical block number
	psn = (pbn * PAGES_PER_BLOCK) + offset; //get physical sector number
	
	fseek(devicefp, PAGE_SIZE * psn, SEEK_SET);
	retval = fread((void*)sectorbuf, SECTOR_SIZE, 1, devicefp);
	if(retval != 1){
		fprintf(stderr, "error: fread\n");
		fclose(devicefp);

		exit(1);
	}

	fseek(devicefp, PAGE_SIZE * (psn + 1), SEEK_SET);

	return;
}

void ftl_write(int lsn, char *sectorbuf){
	int lbn, pbn, psn, offset, retval;
	int free_pbn = free_block[0], free_psn = free_block[0] * PAGES_PER_BLOCK;
	char **buf;
	
	buf = (char**)malloc(sizeof(char*) * PAGES_PER_BLOCK);
	for(int i = 0; i < PAGES_PER_BLOCK; i++){
		buf[i] = (char*)malloc(sizeof(char) * PAGE_SIZE);
		memset(buf[i], 0xFF, PAGE_SIZE);
	}

	lbn = lsn / PAGES_PER_BLOCK; //get logical block number
	offset = lsn % PAGES_PER_BLOCK; //get offset
	pbn = block_table[lbn]; //get physical block number
	psn = (pbn * PAGES_PER_BLOCK) + offset; //get physical sector number

	/*copy*/
	for(int i = 0; i < PAGES_PER_BLOCK; i++){
		retval = dd_read((psn - offset) + i, buf[i]);
		
		if(retval == -1){
			fprintf(stderr, "error: dd_read\n");

			for(int i = 0; i < PAGES_PER_BLOCK; i++){
				free(buf[i]);
			}
			free(buf);

			fclose(devicefp);
			exit(1);
		}
	}
		
	/*paste*/
	for(int i = 0; i < PAGES_PER_BLOCK; i++){
		retval = dd_write(free_psn + i, buf[i]);
		if(retval == -1){
			fprintf(stderr, "error: dd_write\n");

			for(int i = 0; i < PAGES_PER_BLOCK; i++){
				free(buf[i]);
			}
			free(buf);

			fclose(devicefp);
			exit(1);
		}

		memset(buf[i], 0xFF, PAGE_SIZE);
	}

	/*erase*/
	retval = dd_erase(pbn);
	if(retval == -1){
		fprintf(stderr, "error: dd_erase\n");

		for(int i = 0; i < PAGES_PER_BLOCK; i++){
			free(buf[i]);
		}
		free(buf);

		fclose(devicefp);
		exit(1);
	}

	/*write: sectorbuf*/
	retval = dd_write(psn, sectorbuf);
	if(retval == -1){
		fprintf(stderr, "error: dd_write\n");

		for(int i = 0; i < PAGES_PER_BLOCK; i++){
			free(buf[i]);
		}
		free(buf);

		fclose(devicefp);
		exit(1);
	}
	
	/*copy*/
	for(int i = 0; i < PAGES_PER_BLOCK; i++){
		if(i == offset){
			continue;
		}

		retval = dd_read(free_psn + i, buf[i]);
		if(retval == -1){
			fprintf(stderr, "error: dd_read\n");

			for(int i = 0; i < PAGES_PER_BLOCK; i++){
				free(buf[i]);
			}
			free(buf);

			fclose(devicefp);
			exit(1);
		}
	}
	
	/*paste*/
	for(int i = 0; i < PAGES_PER_BLOCK; i++){
		if(i == offset){
			continue;
		}

		retval = dd_write((pbn * PAGES_PER_BLOCK) + i, buf[i]);
		if(retval == -1){
			fprintf(stderr, "error: dd_write\n");

			for(int i = 0; i < PAGES_PER_BLOCK; i++){
				free(buf[i]);
			}
			free(buf);

			fclose(devicefp);
			exit(1);
		}

		memset(buf[i], 0xFF, PAGE_SIZE);
	}

	/*erase: free block*/
	retval = dd_erase(free_pbn);
	if(retval == -1){
		fprintf(stderr, "error: dd_read\n");

		for(int i = 0; i < PAGES_PER_BLOCK; i++){
			free(buf[i]);
		}
		free(buf);
		
		fclose(devicefp);
		exit(1);
	}
	
	for(int i = 0; i < PAGES_PER_BLOCK; i++){
		free(buf[i]);
	}
	free(buf);

	return;
}
