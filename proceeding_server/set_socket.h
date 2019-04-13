#include <sys/types.h>
#include <sys/socket.h>

int server_ready(int *server_fd, struct sockaddr_in *addr, int option, int size, int q_size);
int server_polling(int *server_fd, struct epoll_event *ep_evnt, int limit);


