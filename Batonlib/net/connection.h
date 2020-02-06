#ifndef CONNECTION_H
#define CONNECTION_H
#include "../coroutine.h"
#include "../scheduler.h"
#include "../co_sys_call.h"
#include <functional>

using namespace std;

typedef function<void(connection*)> ConnectedCb;
typedef function<void(connection*, const char*, size_t)> ReceiveCb;
typedef function<void(connection*)> DisconnectedCb;

class connection
{
public:
    scheduler* sch_;
    coroutine* co_;
    int fd_;
    bool end_;

    ConnectedCb ConnectedCb_;
    ReceiveCb ReceiveCb_;
    DisconnectedCb DisconnectedCb_;

    enum statue{connected, connecting, close_by_server_timeout,close_by_server_other,
     close_by_client, disconnected};

    statue connection_statue_;

public:
    connection(int fd, scheduler* sch);
    ~connection();
    void setcoroutine(coroutine* co);

    void set_read_timeout(int ms);
    void set_write_timeout(int ms);

    void setConnectedCallBack(ConnectedCb cb)
    {
        ConnectedCb_ = cb;
    }

    void setReceiveCb(ReceiveCb cb)
    {
        ReceiveCb_ = cb;
    }

    void setDisconnectedCb(DisconnectedCb cb)
    {
        DisconnectedCb_ = cb;
    }

    void send(const void* data, size_t len);

    void send(int fd, const void* data, size_t len);

    void close();

    void run();
};

void* conn_func(void*);


#endif