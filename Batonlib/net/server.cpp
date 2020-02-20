#include "server.h"
#include "../co_sys_call.h"
#include "../tool.h"
#include "../base/Logging.h"

server::server(const char* ip, int port)
    :ip_(ip),
    port_(port),
    sch_(scheduler::get_curr_thr_sch())
{

}

server::~server()
{

}

void server::init()
{

    fd_ = co_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(fd_ >= 0){
        if(port_ != 0){
            int nReuseAddr = 1;
            setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &nReuseAddr, sizeof(nReuseAddr));
            struct sockaddr_in addr;
            SetAddr(ip_, port_, addr);
            int ret = bind(fd_, (struct sockaddr*)&addr, sizeof(addr));
            if(ret != 0){
                co_close(fd_);
                LOG<<"bind error";
                abort();
            }
        }
    }
    LOG<<"bind";
    listen(fd_, 1024);
}

void server::run()
{
    while(1){
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        socklen_t len = sizeof(addr);

        int client_fd = co_accept(fd_, (struct sockaddr*)&addr, &len);
        if(client_fd < 0){
            LOG<<"accept fail";
            continue;
        }

        connection* conn = new connection(client_fd, sch_);
        coroutine* co = new coroutine(sch_, 1024*1024, conn_func, conn);
        conn->setcoroutine(co);
        conn->setConnectedCb(ConnectedCb_);
        conn->setDisconnectedCb(DisconnectedCb_);
        conn->setReceiveCb(ReceiveCb_);
        LOG<<"resume"<<co;
        co->resume();
    }
}

void server::start()
{
    coroutine* co = new coroutine(sch_, 1024*1024, server_func, this);
    co->resume();
}

void* server_func(void* arg)
{
    server* ser = (server*)arg;
    ser->init();
    ser->run();
}