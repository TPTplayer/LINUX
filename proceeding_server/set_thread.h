#include <sys/epoll.h>
#include <sys/types.h>

typedef struct _arg_struct{
	int epfd;
	int limit;
	struct epoll_event *ep_evnt;
}arg_struct;

int background_polling(int epfd, struct epoll_event *ep_evnt, int limit);


