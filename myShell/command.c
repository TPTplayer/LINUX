#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "error_util.h"
#include "command.h"
#include "execute.h"

extern int dummy;

int cmd_input(char *cmd){ //프롬프트 입력
	char *search;

	cmd[strlen(cmd) - 1] = 0; //개행문자 없애기
	if(!strcmp(cmd, "exit")){  
		return 0; //exit일 경우 0 반환
	}
	
	if((search = strstr(cmd, "cd ")) != NULL){ //명령어: cd
		cmd_cd(cmd);
	}
	else if(cmd[0] == '.' || cmd[1] == '/'){ //실행
		cmd_execute(cmd);
	}
	else{ //그 외(엔터 제외)
		puts("This command isn't supported");
	}

	return 1;
}

void cmd_cd(char *cmd){
	int retval = 0;
	char *path;
	
	path = (char*)malloc(sizeof(char) * CMD_BUF);
	if(path == NULL){
		error_handler(errno, "malloc");
		exit(1); 
	}
	memset(path, 0, CMD_BUF);

	for(int i = 3; i < CMD_BUF; i++){ //cd는 2byte, 공백은 1byte. 
		if(cmd[i] == 0){
			break;
		}	
		path[i - 3] = cmd[i]; //이동할 경로만 path에 집어넣는다
	}

	retval = chdir(path); //작업 디렉터리 바꾸기
	if(retval == -1){
		error_handler(errno, "shell: cd"); //shell상의 cd명령어를 수행하다 에러가 발생했으므로 함수명을 인자로 주지 않는다
	}
}

void cmd_execute(char *cmd){
	char *path, *redirect_file, **op_arr, *op_buf;
	int cmd_offset = 2, cmd_len = 0, flag_background = 0, flag_redirect = 0, flag_op = 0, num_op = -1, op_end = 0;
	
	if(!strcmp(cmd, "./")){ //"./"만 입력으로 들어왔을 경우
		puts("shell: ./: is a directory");
		return;
	}

	path = (char*)malloc(sizeof(char) * CMD_BUF); //실행할 파일명
	redirect_file = (char*)malloc(sizeof(char) * CMD_BUF); //리다이렉트 파일명
	op_buf = (char*)malloc(sizeof(char) * CMD_BUF); //옵션 임시저장용 버퍼
	if(path == NULL || redirect_file == NULL || op_buf == NULL){ 
		error_handler(errno, "malloc");
		exit(1);
	}
	memset(path, 0, CMD_BUF);
	memset(redirect_file, 0, CMD_BUF);
	memset(op_buf, 0, CMD_BUF);

	op_arr = (char**)malloc(sizeof(char*) * CMD_BUF); //옵션 전달용 버퍼의 포인터 저장용 공간할당
	if(op_arr == NULL){
		error_handler(errno, "malloc");
		exit(1);
	}
	for(int i = 0; i < CMD_BUF; i++){
		op_arr[i] = (char*)malloc(sizeof(char) * CMD_BUF); //옵션 저장용 공간할당
		if(op_arr[i] == NULL){
			error_handler(errno, "malloc");
			exit(1);
		}
		memset(op_arr[i], 0, CMD_BUF);
	}
	
	/*실행파일명(경로) 구하기(시작)*/
	for(cmd_len = cmd_offset; cmd_len < CMD_BUF; cmd_len++){ //cmd_offset: 2("./"는 2byte, 공백 없음)
		if(cmd[cmd_len] == '&'){ //백그라운드 옵션이 존재할 경우
			flag_background = 1;
			continue; //&는 파일명에 포함되지 않는다
		}
		else if(cmd[cmd_len] == ' '){ //옵션 혹은 리다이렉션의 존재 가능성이 있음
			if(cmd[cmd_len + 1] == '>'){ //옵션 없이 리다이렉션만 존재할 경우
				flag_redirect = 1;
				cmd_offset = cmd_len + 3; //cmd_len: 지금까지 읽은 명령어의 길이, cmd_offset: 읽기 시작할 주소 (" > "은 3byte)
				break;
			}
			else{ //옵션이 존재할 경우
				flag_op = 1;
				cmd_offset = cmd_len + 1; //공백(1byte)을 두고 옵션이 위치하므로
				break;
			}
		}
		else if(cmd[cmd_len] == 0){ //옵션 혹은 리다이렉션이 존재하지 않을 시
			break;
		}

		path[cmd_len - cmd_offset] = cmd[cmd_len]; 
	}
	/*실행파일명(경로) 구하기(끝)*/
	
	/*옵션 구하기(시작)*/
	if(flag_op == 1){
		for(num_op = 0; num_op < CMD_BUF; num_op++){ //옵션의 개수 카운트(num_op: 옵션의 갯수)
			for(int i = cmd_offset; i < CMD_BUF; i++){
				if(cmd[i] == ' '){ //옵션 1개가 끝나는 지점
					cmd_offset = cmd_offset + (i - cmd_offset) + 1; //원래의 offset에 지금까지 읽은 바이트(i - offset)을 더한다.(offset 갱신)
					//1을 더해 다음 바이트를 검사
					if(cmd[cmd_offset] == '>'){ //리다이렉션 문자가 발견될 경우 옵션은 더 이상 존재할 수 없다
						flag_redirect = 1;
						cmd_offset += 2; //>앞에 공백이 존재하므로 2byte만큼 offset을 더해 리다이렉션 파일명의 시작지점으로 offset을 갱신
						op_end = 1; //옵션의 끝 표시
						break;
					}
					else{ //옵션이 이어질시
						break;
					}
				}
				else if(cmd[i] == 0){ //옵션이 끝날 시
					op_end = 1; //옵션의 끝 표시
					break;
				}
				op_buf[i - cmd_offset] = cmd[i]; //임시버퍼에 1byte씩 읽은 옵션을 옮긴다
			}

			memmove(op_arr[num_op], op_buf, CMD_BUF); //num_op번째 옵션에 op_buf의 옵션 옮기기
			memset(op_buf, 0, CMD_BUF);
			
			if(op_end == 1){
				break; //옵션이 끝났을 경우 나간다
			}
		}
	}
	/*옵션 구하기(끝)*/

	/*리다이렉트 파일 구하기(시작)*/
	if(flag_redirect == 1){ 
		for(int i = cmd_offset; i < CMD_BUF; i++){ //offset은 옵션 구하기의 마지막에 이미 갱신했으므로
			if(cmd[i] == 0){
				break;
			}
			redirect_file[i - cmd_offset] = cmd[i];
		}
	}
	/*리다이렉트 파일 구하기(끝)*/

	if(flag_background == 1){ //백그라운드 옵션이 존재할 경우
		if(flag_op == 0 && flag_redirect == 0){
			excution_background(path, NULL, NULL, 0);
		}
		else if(flag_op == 0 && flag_redirect == 1){
			excution_background(path, redirect_file, NULL, 0);
			if(dup2(dummy, STDOUT_FILENO) == -1){ //표준출력 되돌리기
				error_handler(errno, "dup2");
				exit(1);
			}

			close(dummy); //더미 닫기
			remove("dummy_file"); //더미 삭제 
		}
		else if(flag_op == 1 && flag_redirect == 0){
			excution_background(path, NULL, op_arr, num_op);
		}
		else if(flag_op == 1 && flag_redirect == 1){
			excution_background(path, redirect_file, op_arr, num_op);
			if(dup2(dummy, STDOUT_FILENO) == -1){
				error_handler(errno, "dup2");
				exit(1);
			}

			close(dummy);
			remove("dummmy_file");
		}
	}
	else{ //백그라운드 옵션이 존재하지 않을 경우
		if(flag_op == 0 && flag_redirect == 0){
			excution_foreground(path, NULL, NULL, 0);
		}	
		else if(flag_op == 0 && flag_redirect == 1){
			excution_foreground(path, redirect_file, NULL, 0);
		}
		else if(flag_op == 1 && flag_redirect == 0){
			excution_foreground(path, NULL, op_arr, num_op + 1);
		}
		else if(flag_op == 1 && flag_redirect == 1){
			excution_foreground(path, redirect_file, op_arr, num_op + 1);
		}
	}
	
	free(path);
	free(redirect_file);
	free(op_buf); 
	for(int i = 0; i < num_op; i++){ 
		free(op_arr[i]);
	}
	free(op_arr);

	return;
}
