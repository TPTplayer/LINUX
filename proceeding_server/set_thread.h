typedef struct _POLL_ARG{
	int epfd;
	struct epoll_event *evnt;
	int maxevnts;
	int server_fd;
}poll_arg;

typedef struct _POLL_RT{
	int *client_fd;
	int que;
}poll_rt;

void *connect_polling(void *argument);
void *rt_polling(void *argument);
void *check_hangup(void *);

