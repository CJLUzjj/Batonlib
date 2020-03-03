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

    while(1){
        char buf[1024*16];
        int len = co_read(fd, buf, sizeof(buf));
        //cout<<fd<<" read:"<<endl<<buf<<endl;


        if(len > 0){
            string index = "<title>hello</title>helloworld";
            string out = "HTTP/1.0 200 OK\r\n";
            out += "Content-Type: text/html; charset=UTF-8\r\n";
            out += "Content-Length: " + to_string(index.size()) + "\r\n";
            out += "Server: ZhuJunjie's Web Server\r\n";
            out += "\r\n";
            out += index;
            len = co_write(fd, out.c_str(), out.size());
            co_close(fd);
            break;
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
    //LOG<<"end";
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
    if (daemon(1, 0) < 0) {
		return -1;
	}
    listen_inf* inf = new listen_inf("0.0.0.0", 80);
    scheduler sch;
    coroutine co(&sch, 1024*1024, accept_func, (void*)inf);
    //LOG<<"resume "<<(long)&co;
    LOG<<"resume "<<&co;
    co.resume();
    //cout<<"loop"<<endl;
    sch.event_loop(NULL, NULL);
}
