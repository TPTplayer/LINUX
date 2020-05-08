#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

#include "func_path.h"
#include "error_util.h"

void chWorkDIR(){ //작업디렉터리를 home으로 변경
	int retval = 0;

	retval = chdir("/home"); 
	if(retval == -1){
		error_handler(errno, "chdir");
		exit(1);
	}
}

void printPrompt(){
	gid_t grp_id; 
	char user[DIR_BUF], grp[DIR_BUF], *user_p, *buf;
	struct passwd *pwd;
	struct group *grp_entry;
	
	buf = (char*)malloc(sizeof(char) * DIR_BUF);
	if(buf == NULL){
		error_handler(errno, "malloc");
		exit(1);
	}
	
	memset(user, 0, DIR_BUF);
	memset(grp, 0, DIR_BUF);
	memset(buf, 0, DIR_BUF);

	user_p = user; //getlogin함수는 포인터(문자열의 시작주소)를 반환하므로 넣는다.
	/*그대로 넣으면 l-value에러 발생*/
	user_p = getlogin(); //로그인된 사용자 이름 구하기
	pwd = getpwnam(user_p); //로그인 이름을 인자로 passwd구조체 구하기
	memmove(user, pwd->pw_name, DIR_BUF); //출력버퍼에 옮기기
	
	grp_id = getgid(); //그룹 id구하기
	grp_entry = getgrgid(grp_id); //group구조체 구하기
	memmove(grp, grp_entry->gr_name, DIR_BUF); //구조체 내의 그룹 이름을 출력버퍼로 옮긴다
	
	getcwd(buf, DIR_BUF); //현재 작업 디렉터리 구하기

	printf("%s@%s:%s# ", grp, user, buf);
	free(buf);
}
