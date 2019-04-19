#include <sys/epoll.h>
#define QUE 10

int set_socket(int *serverf_fd, int port_num);
int set_polling(int *epfd, int *server_fd, struct epoll_event *ep_evnt);
