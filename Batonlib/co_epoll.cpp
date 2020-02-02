#include "co_epoll.h"
#include "base/Logging.h"

co_epoll::co_epoll(int size)
    :events(size)
{
    epoll_fd_ = epoll_create(5);
    active_link_ = new time_link;
    timeout_link_ = new time_link;
}

co_epoll::~co_epoll()
{
    close(epoll_fd_);
    delete active_link_;
    delete timeout_link_;
}

int co_epoll::add(int fd, __uint32_t event, co_epoll_item* item)
{
    if(item_map_.find(fd) != item_map_.end()){
        LOG<<"already have fd";
        return -1;
    }
    epoll_event new_event;
    new_event.data.ptr = item;
    item_map_[fd] = item;
    new_event.events = event;
    epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &new_event);
    return 0;
}

int co_epoll::del(int fd)
{
    if(item_map_.find(fd) == item_map_.end()){
        LOG<<"del:couldn't find fd";
        return -1;
    }
    epoll_event new_event;
    new_event.data.ptr = item_map_[fd];
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &new_event);
    item_map_.erase(fd);
    return 0;
}

int co_epoll::mod(int fd, __uint32_t event)
{
    if(item_map_.find(fd) == item_map_.end()){
        LOG<<"mod:couldn't find fd";
        return -1;
    }
    epoll_event new_event;
    new_event.data.ptr = item_map_[fd];
    new_event.events = event;
    epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &new_event);
    return 0;
}

int co_epoll::poll(int ms)
{
    int number;
    number = epoll_wait(epoll_fd_, events.data(), events.size(), ms);
    return number;
}