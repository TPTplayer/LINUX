#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

#include "func_path.h"
#include "error_util.h"
#include "command.h"

int main(void){
	char *cmd_buf;
	int retval = 0;
	
	signal(SIGINT, SIG_IGN); //interrupt(ctrl + C)시 무시

	cmd_buf = (char*)malloc(sizeof(char) * CMD_BUF);
	if(cmd_buf == NULL){
		error_handler(errno, "malloc");
		exit(1);
	}
	memset(cmd_buf, 0, CMD_BUF);

	chWorkDIR(); //작업 디렉터리를 home디렉터리로 바꾸기(최초 1번 실행)
	while(1){
		printPrompt(); //프롬프트 출력
		fgets(cmd_buf, CMD_BUF, stdin);
		
		if(cmd_buf[0] == '\n'){ //프롬프트의 입력이 개행문자(엔터)일 경우 다시 입력
			continue;
		}	

		retval = cmd_input(cmd_buf);
		if(retval == 0){ 
			break;
		}
	}
	
	free(cmd_buf);
	return 0;
}
