#include "connection.h"
#include "../base/Logging.h"

connection::connection(int fd, scheduler* sch)
    :fd_(fd),
    sch_(sch),
    end_(false)
{
    connection_statue_ = disconnected;
    LOG<<"constructed connection"<<this;
}

connection::~connection()
{
    LOG<<"distructed connection"<<this;
}

//在创建协程后必须为该连接指定创建的协程
void connection::setcoroutine(coroutine* co)
{
    co_ = co;
    co_->setconn(this);
}

void connection::set_read_timeout(int ms)
{
    socket_inf* inf = g_socket_inf[fd_];
    inf->read_timeout.tv_sec = ms/1000;
    inf->read_timeout.tv_usec = (ms%1000)*1000;
}

void connection::set_write_timeout(int ms)
{
    socket_inf* inf = g_socket_inf[fd_];
    inf->write_timeout.tv_sec = ms/1000;
    inf->write_timeout.tv_usec = (ms%1000)*1000;
}

void connection::run()
{
    connection_statue_ = connected;
    ConnectedCb_(this);

    connection_statue_ = connecting;
    
    char buf[1024*2];
    
    while(!end_){
        int len = co_read(fd_, buf, sizeof(buf));

        //LOG<<"co_read return len:"<<len;
        if(len > 0){
            ReceiveCb_(this, buf, len);
        }

        if(len <= 0){
            if(len != TIME_OUT){
                if(errno == EAGAIN)
                    continue;
                if(errno == EINTR)
                    continue;
            }
            if(len == TIME_OUT){
                connection_statue_ = close_by_server_timeout;
                break;
            }
            connection_statue_ = close_by_client;
            break;
        }
    }
    
    if(end_ && connection_statue_ != close_by_client){
        connection_statue_ = close_by_server_other;
    }

    DisconnectedCb_(this);

    connection_statue_ = disconnected;

    co_close(fd_);
}

void connection::send(const void* data, size_t len)
{
    co_write(fd_, data, len);
}

void connection::send(int fd, const void* data, size_t len)
{
    co_write(fd, data, len);
}

void connection::close()
{
    end_ = true;
}

void* conn_func(void* conn)
{
    connection* conn_ = (connection*)conn;
    conn_->run();
    conn_->end_ = true;
    return NULL;
}