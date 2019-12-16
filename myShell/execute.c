#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include "error_util.h"
#include "execute.h"
#include "func_path.h"
#include "command.h"

int dummy = 0;

void excution_background(char *path, char *redirect_file, char **op, int num_op){
	pid_t shell, execution;
	char process_argv[CMD_BUF][CMD_BUF] = {0, }, *process_argv_pt[CMD_BUF] = {NULL, }; //2차원배열은 0, 포인터배열은 NULL로 초기화
	int redirect_fd = 0; //redirect_fd: 리다이렉트할 파일, dummy: 표준출력 백업용

	if(num_op != 0){ //옵션이 존재할 경우
		memmove(process_argv[0], path, CMD_BUF);
		process_argv_pt[0] = process_argv[0]; //포인터에 주소를 지정해준다
		
		for(int i = 0; i < num_op + 1; i++){
			memmove(process_argv[i + 1], op[i], CMD_BUF); //배열에 옵션 집어넣기
			process_argv_pt[i] = process_argv[i]; //포인터에 주소를 지정하여 배열과 연결
		}
	}

	if(redirect_file != NULL){ //리다이렉트 파일이 존재할 경우
		dummy = open("dummy_file", O_WRONLY | O_CREAT, S_IRWXU); //더미 파일 열기(고유 파일 디스크립터 확보용)
		if(dummy == -1){
			error_handler(errno, "open");
			exit(1);
		}

		redirect_fd = open(redirect_file, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU); //리다이렉트 파일 열기
		if(redirect_fd == -1){
			error_handler(errno, "shell: >");
			return;
		}

		if(dup2(STDOUT_FILENO, dummy) == -1){ //표준출력 파일 디스크립터(1)백업
			error_handler(errno, "dup2");
			exit(1);
		}

		if(dup2(redirect_fd, STDOUT_FILENO) == -1){ //표준출력을 리다이렉트 파일으로 리다이렉트
			error_handler(errno, "dup2");
			exit(1);
		}
	}

	shell = getpid(); //쉘의 pid구하기
	fork(); //새 프로세스 생성

	if(getpid() != shell){ //자식 프로세스 pid구하기
		execution = getpid();
	}
	else{
		return; //쉘은 바로 리턴
	}
	/*
	exec계열의 함수로 다른 프로세스를 실행할 땐, 반드시 인자가 NULL으로 종료되어야 한다.

	execv: vector입출력으로 인자를 전달(포인터 배열을 인자로 받는다. 이 경우 인자의 끝을 나타내는 포인터 배열은 NULL이 되어야 한다.)
	execl: list로 인자를 전달(포인터를 인자로 나열시킨다. 이 경우 맨 끝의 인수는 NULL을 주어야 한다)

	인자의 순서(vector입출력의 경우 포인터의 순서): <실행할 바이너리파일 위치> <실행할 바이너리파일 이름> <옵션 1> <옵션 2>... <NULL> 
	*/

	if(getpid() == execution){ //자식 프로세스의 경우
		if(redirect_file != NULL){
			setsid();
		}
		
		signal(SIGINT, SIG_IGN); //사용자 인터럽트 무시

		if(num_op != 0){ //옵션이 존재하는 경우
			if(execv(path, process_argv_pt) == -1){ //vector입출력을 하기 때문에 포인터 배열으로 전달
				error_handler(errno, "shell: ./");
				exit(1);
			}
		}
		else{ //옵션이 존재하지 않는 경우
			if(execl(path, path, NULL) == -1){ //그대로 list로 전달 (첫 번째 인자로 실행파일, 두 번째로 바이너리 이름 전달, 반드시 NULL으로 종료) 
				error_handler(errno, "shell: ./");
				exit(1);
			}
		}
	}
}

void excution_foreground(char *path, char *redirect_file, char **op, int num_op){ //기본적으로 위의 함수와 거의 같다. 다른 점은 wait로 자식 프로세스가 끝나길 기다리며 블록된다는 점.
	pid_t shell, execution;
	char process_argv[CMD_BUF][CMD_BUF] = {0, }, *process_argv_pt[CMD_BUF] = {NULL, }; 
	int exit_status = 0, redirect_fd = 0;

	if(num_op != 0){
		memmove(process_argv[0], path, CMD_BUF);
		process_argv_pt[0] = process_argv[0];

		for(int i = 0; i < num_op + 1; i++){
			memmove(process_argv[i + 1], op[i], CMD_BUF);
			process_argv_pt[i] = process_argv[i];
		}
	}

	if(redirect_file != NULL){
		dummy = open("dummy_file", O_WRONLY | O_CREAT, S_IRWXU);
		if(dummy == -1){
			error_handler(errno, "open");
			exit(1);
		}

		redirect_fd = open(redirect_file, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);
		if(redirect_fd == -1){
			error_handler(errno, "shell: >");
			return;
		}
		
		if(dup2(STDOUT_FILENO, dummy) == -1){
			error_handler(errno, "dup2");
			exit(1);
		}

		if(dup2(redirect_fd, STDOUT_FILENO) == -1){
			error_handler(errno, "dup2");
			exit(1);
		}
	}

	shell = getpid();
	fork();

	if(getpid() != shell){
		execution = getpid();
	}

	if(getpid() == execution){
		signal(SIGINT, SIG_DFL);
		if(num_op != 0){
			if(execv(path, process_argv_pt) == -1){
				error_handler(errno, "shell: ./");
				exit(1);
			}
		}
		else{
			if(execl(path, path, NULL) == -1){
				error_handler(errno, "shell: ./");
				exit(1);
			}
		}
	}

	wait(&exit_status); //wait를 통해 자식 프로세스가 끝나길 기다린다. exit_status는 매크로를 통해 값을 구할 수 있으나 여기선 사용하지 않는다.
	if(redirect_file != NULL){ //리다이렉트 파일이 존재했을 경우 
		if(dup2(dummy, STDOUT_FILENO) == -1){ //표준출력을 다시 터미널로 되돌린다.
			error_handler(errno, "dup2");
			exit(1);
		}
		
		close(dummy); //더미 파일 디스크립터를 닫고
		close(redirect_fd); //리다이렉트용 파일 디스크립터를 닫은 뒤

		remove("dummy_file"); //더미 파일을 삭제한다
	}
	return;
}
