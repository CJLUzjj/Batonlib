#ifndef SERVER_H
#define SERVER_H
#include "../scheduler.h"
#include "connection.h"

using namespace std;

typedef function<void(connection*)> ConnectedCb;
typedef function<void(connection*, const char*, size_t)> ReceiveCb;
typedef function<void(connection*)> DisconnectedCb;

class server
{
private:
    const char* ip_;
    int port_;
    int fd_;
    scheduler* sch_;

    ConnectedCb ConnectedCb_;
    ReceiveCb ReceiveCb_;
    DisconnectedCb DisconnectedCb_;
public:
    server(const char* ip, int port);
    ~server();

    void start();

    void init();
    void run();

    void setConnectedCb(ConnectedCb cb)
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
};

void* server_func(void*);
#endif