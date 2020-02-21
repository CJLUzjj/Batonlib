#include "scheduler.h"
#include "co_sys_call.h"
#include "tool.h"
#include "base/Logging.h"
#include "channel.h"
#include <memory>
#include <string>
#include <map>
using namespace std;

struct client{
    int fd;
    string name;
    channel<string> msg;
    bool end;
    ~client()
    {
        cout<<"user"<<name<<" delete"<<endl;
    }
};

typedef shared_ptr<client> clientptr;

channel<string>* message;

vector<clientptr> online_clients;

void* user_read(void* arg)
{
    clientptr cli = *(clientptr*)arg;
    char buf[1024];
    while(1){
        memset(buf, 0, 1024);
        int len = co_read(cli->fd, buf, 1024);
        if(len > 0)
            cout<<"user"<<cli->name<<" read:"<<buf<<endl;

        if(len <= 0){
            if(len != TIME_OUT){
                if(errno == EAGAIN)
                    continue;
                if(errno == EINTR)
                    continue;
            }
            co_close(cli->fd);
            int index = atoi(cli->name.c_str());
            online_clients[index]->msg.close();
            break;
        }
        //if(buf == "!quit")
            //goto close;
        if(!message->put(string(buf)))
            break;
    }
}

void* handle_conn(void* arg)
{
    int fd = *((int*)arg);
    clientptr cli = make_shared<client>();
    cli->end = false;
    int flag = false;
    int id;
    for(int i = 0; i < online_clients.size(); i++){
        if(online_clients[i] == NULL){
            online_clients[i] = cli;
            id = i;
            flag = true;
            break;
        }
    }
    if(!flag){
        online_clients.push_back(cli);
        id = online_clients.size()-1;
    }
    cli->name += to_string(id);
    cli->fd = fd;
    string login = "login" + cli->name;
    if(!message->put(login))
        return NULL;

    coroutine* co = new coroutine(scheduler::get_curr_thr_sch(), 1024*1024, user_read, &cli);
    co->resume();

    while(1){
        string msg;
        if(!cli->msg.get(msg)){
            break;
        }
        msg += "\n";
        int ret = co_write(fd, msg.c_str(), msg.size());
    }
}

void* manager(void*)
{
    while(1){
        string msg;
        if(!message->get(msg))
            break;
        for(int i = 0; i < online_clients.size(); i++){
            if(online_clients[i]){
                if(!online_clients[i]->msg.put("user"+online_clients[i]->name+" say:"+msg)){
                    online_clients[i] = NULL;
                    continue;
                }
            }
        }
    }
}

void* accept_func(void* arg)
{
    int* port = (int*)arg;

    int fd = co_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(fd >= 0){
        if(*port != 0){
            int nReuseAddr = 1;
            setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &nReuseAddr, sizeof(nReuseAddr));
            struct sockaddr_in addr;
            SetAddr("0.0.0.0", *port, addr);
            int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
            if(ret != 0){
                co_close(fd);
                abort();
            }
        }
    }
    listen(fd, 1024);

    coroutine* manager_co = new coroutine(scheduler::get_curr_thr_sch(), 1024*1024, manager, NULL);
    manager_co->resume();

    while(1){
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        socklen_t len = sizeof(addr);

        int client_fd = co_accept(fd, (struct sockaddr*)&addr, &len);
        if(fd < 0){
            cout<<"accept fail"<<endl;
            continue;
        }

        coroutine* co = new coroutine(scheduler::get_curr_thr_sch(),1024*1024, handle_conn, (void*)&client_fd);
        co->resume();
    }
}

int main()
{
    scheduler sch;
    //注意：不要将message作为全局变量初始化，这样message会在sch之前创建，channel不能指定所属的scheduler
    message = new channel<string>;
    int port = 9981;
    coroutine co(&sch, 1024*1024, accept_func, (void*)&port);
    co.resume();
    sch.event_loop(NULL, NULL);
}