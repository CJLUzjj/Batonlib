#ifndef __CO_EPOLL_H__
#define __CO_EPOLL_H__
#include "time_wheel.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <map>

using namespace std;

class coroutine;

struct co_epoll_item
{
    int fd;

    bool is_timeout;

    coroutine* co;

    time_item* t_item;
};

class co_epoll
{
public:
    int epoll_fd_;
    vector<struct epoll_event> events;
    map<int, co_epoll_item*> item_map_;
    time_link* active_link_;
    time_link* timeout_link_;
public:
    co_epoll(int size);
    ~co_epoll();

    int add(int fd, __uint32_t event, co_epoll_item* item);

    int del(int fd);

    int mod(int fd, __uint32_t event);

    int poll(int ms);
};

#endif