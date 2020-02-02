#include "co_sys_call.h"
#include "scheduler.h"
#include "tool.h"
#include "base/Logging.h"
#include <iostream>

using namespace std;

struct listen_inf{
    const char* ip;
    int port;
    listen_inf(const char* ip_, int port_)
    {
        ip = ip_;
        port = port_;
    }
};

void* echo_func(void* arg)
{
    int fd = *(int*)arg;

    char buf[1024*2];

    while(1){
        int len = co_read(fd, buf, sizeof(buf));

        LOG<<"co_read return len:"<<len;

        if(len > 0){
            len = co_write(fd, buf, len);
        }

        if(len <= 0){
            if(len != TIME_OUT){
                if(errno == EAGAIN)
                    continue;
                if(errno == EINTR)
                    continue;
            }
            co_close(fd);
            LOG<<"close client";
            break;
        }
    }
    LOG<<"end";
    return NULL;
}

void* accept_func(void* arg)
{
    listen_inf* inf = (listen_inf*)arg;

    int fd = co_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(fd >= 0){
        if(inf->port != 0){
            int nReuseAddr = 1;
            setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &nReuseAddr, sizeof(nReuseAddr));
            struct sockaddr_in addr;
            SetAddr(inf->ip, inf->port, addr);
            int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
            if(ret != 0){
                co_close(fd);
                LOG<<"bind error";
                abort();
            }
        }
    }
    LOG<<"bind";
    listen(fd, 1024);

    LOG<<"accept coroutine";

    while(1){
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        socklen_t len = sizeof(addr);

        int client_fd = co_accept(fd, (struct sockaddr*)&addr, &len);
        if(fd < 0){
            cout<<"accept fail"<<endl;
            continue;
        }

        coroutine* co = new coroutine(scheduler::get_curr_thr_sch(),1024*1024, echo_func, (void*)&client_fd);
        //LOG<<"resume "<<(long)co;
        LOG<<"resume "<<co;
        co->resume();
    }
}

int main(int argc, char** argv)
{
    listen_inf* inf = new listen_inf("0.0.0.0", 9981);
    scheduler sch;
    coroutine co(&sch, 1024*1024, accept_func, (void*)inf);
    //LOG<<"resume "<<(long)&co;
    LOG<<"resume "<<&co;
    co.resume();
    //cout<<"loop"<<endl;
    sch.event_loop(NULL, NULL);
}