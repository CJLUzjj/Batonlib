#ifndef CO_SYS_CALL_H
#define CO_SYS_CALL_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <time.h>
using namespace std;

#define TIME_OUT -10086

struct socket_inf;

extern map<int, socket_inf*> g_socket_inf;

struct socket_inf{

    struct sockaddr_in dest;
    int domain;
    int fd;
    
    bool istimeout;

    struct timeval read_timeout;
    struct timeval write_timeout;

    socket_inf(int fd);

    ~socket_inf();
};

int co_socket(int domain, int type, int protocol);

int co_accept(int fd, struct sockaddr* address, socklen_t* address_len);

int co_close(int fd);

ssize_t co_read(int fd, void* buf, size_t nbyte);

ssize_t co_write(int fd, const void* buf, size_t nbyte);

ssize_t co_send(int socket, const void* buf, size_t length, int flags);

ssize_t co_recv(int socket, void* buffer, size_t length, int flags);

int co_register(int socket, __int32_t events, int timeout);

int co_setsockopt(int fd, int level, int option_name, const void* option_value, socklen_t option_len);

//int fcntl(int fildes, int cmd, ...);

#endif