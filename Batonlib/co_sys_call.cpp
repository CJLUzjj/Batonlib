#include "co_sys_call.h"
#include "scheduler.h"
#include "base/Logging.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

//所有函数未进行错误检查,谨慎使用
map<int, socket_inf*> g_socket_inf;

socket_inf::socket_inf(int fd)
{
    this->fd = fd;
    read_timeout.tv_sec = 30;
    read_timeout.tv_usec = 0;
    write_timeout.tv_sec = 30;
    write_timeout.tv_usec = 0;
    g_socket_inf[fd] = this;

    istimeout = false;
}
socket_inf::~socket_inf()
{
    g_socket_inf.erase(fd);
    //cout<<"distracted"<<endl;
}

int co_socket(int domain, int type, int protocol)
{
    int fd = socket(domain, type, protocol);

    if(fd < 0){
        return fd;
    }

    socket_inf* inf = new socket_inf(fd);
    inf->domain = domain;
    
    int flag = fcntl(fd, F_GETFL, 0);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);

    return fd;
}

int co_accept(int fd, struct sockaddr* address, socklen_t* address_len)
{
    //LOG<<"co_accept fd:"<<fd;
    int regret = co_register(fd, EPOLLIN|EPOLLERR|EPOLLHUP, -1);

    int client_fd = accept(fd, address, address_len);
    socket_inf* inf = new socket_inf(client_fd);
    
    //将描述符设置为非阻塞
    int flag = fcntl(client_fd, F_GETFL, 0);
    flag |= O_NONBLOCK;
    fcntl(client_fd, F_SETFL, flag);

    return client_fd;
}

int co_connect(int fd, const struct sockaddr* address, socklen_t address_len)
{
    int ret = connect(fd, address, address_len);

    if(ret == 0)return ret;

    if(!(ret < 0 && errno == EINPROGRESS))return ret;

    bool timeout = false;

    for(int i = 0; i < 3; i++){//时间轮最大只能等60秒
        timeout = co_register(fd, EPOLLIN|EPOLLOUT, 25000);
        if(!timeout)
            break;
    }
    if(timeout){
        errno = ETIMEDOUT;
        return TIME_OUT;
    }

    int error = 1;
    socklen_t len;
    ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len);
    if(ret < 0){
        return -1;
    }
    if(error){
        errno = error;
        return -1;
    }
    return 0;
}

int co_close(int fd)
{
    //LOG<<"co_close fd:"<<fd;
    if(g_socket_inf.find(fd) == g_socket_inf.end()){
        cout<<"no find socket_inf fd="<<fd<<endl;
    }
    //cout<<"find socket_inf fd="<<fd<<endl;
    socket_inf* inf = g_socket_inf[fd];
    delete inf;
    //cout<<"delete inf"<<endl;
    return close(fd);
}

ssize_t co_read(int fd, void* buf, size_t nbyte)
{
    //LOG<<"co_read fd:"<<fd;
    socket_inf* inf = g_socket_inf[fd];

    int timeout = inf->read_timeout.tv_sec*1000
                    + inf->read_timeout.tv_usec/1000;
    
    int regret = co_register(fd, EPOLLIN|EPOLLERR|EPOLLHUP, timeout);

    if(regret){
        return TIME_OUT;
    }

    ssize_t readret = read(fd, buf, nbyte);

    return readret;
}

ssize_t co_write(int fd, const void* buf, size_t nbyte)
{
    //LOG<<"co_write fd:"<<fd;
    socket_inf* inf = g_socket_inf[fd];
    size_t write_size = 0;
    int timeout = inf->write_timeout.tv_sec*1000
                + inf->write_timeout.tv_usec/1000;

    //cout<<"write:"<<endl<<buf<<endl;

    ssize_t write_ret = write(fd, (const char*)buf + write_size, nbyte - write_size);

    if(write_ret <= 0){
        return write_ret;
    }

    if(write_ret > 0){
        write_size += write_ret;
    }

    while(write_size < nbyte){
        int regret = co_register(fd, EPOLLOUT|EPOLLERR|EPOLLHUP, timeout);

        if(regret){
            return TIME_OUT;
        }

        write_ret = write(fd, (const char*)buf + write_size, nbyte - write_size);

        if(write_ret <= 0){
            break;
        }

        write_size += write_ret;
    }
    if(write_ret <= 0 && write_size == 0){
        return write_ret;
    }
    return write_size;
}

ssize_t co_send(int socket, const void* buf, size_t length, int flag)
{
    //LOG<<"co_send fd:"<<socket;
    socket_inf* inf = g_socket_inf[socket];
    size_t write_size = 0;
    int timeout = inf->write_timeout.tv_sec*1000
                + inf->write_timeout.tv_usec/1000;

    ssize_t write_ret = send(socket, (const char*)buf + write_size, length - write_size, flag);

    if(write_ret == 0){
        return write_ret;
    }

    if(write_ret > 0){
        write_size += write_ret;
    }

    while(write_size < length){
        int regret = co_register(socket, EPOLLOUT|EPOLLERR|EPOLLHUP, timeout);

        if(regret){
            return TIME_OUT;
        }

        write_ret = send(socket, (const char*)buf + write_size, length - write_size, flag);

        if(write_ret <= 0){
            break;
        }

        write_size += write_ret;
    }
    if(write_ret <= 0 && write_size == 0){
        return write_ret;
    }
    return write_size;    
}

ssize_t co_recv(int socket, void* buffer, size_t length, int flag)
{
    //LOG<<"co_recv fd:"<<socket;
    socket_inf* inf = g_socket_inf[socket];

    int timeout = inf->read_timeout.tv_sec*1000
                    + inf->read_timeout.tv_usec/1000;
    
    int regret = co_register(socket, EPOLLIN|EPOLLERR|EPOLLHUP, timeout);

    if(regret){
        return TIME_OUT;
    }

    ssize_t readret = recv(socket, buffer, length, flag);

    return readret;    
}

int co_setsockopt(int fd, int level, int option_name, const void* option_value, socklen_t option_len)
{

}

int co_register(int socket, __int32_t events, int timeout)
{
    //LOG<<"co_register fd:"<<socket;
    scheduler* sch = scheduler::get_curr_thr_sch();
    int ret = sch->register_event(socket, events, timeout);
    return ret;
}