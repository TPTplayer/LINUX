#define PATH_LEN 512
#define TIME_LEN 20

void error_handler(int err, char *func);
int stdio_err_redirection(char *file_log, char *file_err);
int get_current_time(char *buf);
int set_config(void);
int read_config(char *path, char *file_log, char *file_err);
