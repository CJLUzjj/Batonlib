#include "time_wheel.h"
#include "tool.h"
#include "base/Logging.h"
#include <iostream>

time_item::time_item(unsigned long long ex_t, timeout_func func, void* func_arg)
{
    next = NULL;
    prev = NULL;
    t_link = NULL;
    ExpireTime = ex_t;
    process = func;
    arg = func_arg;
    item = NULL;
    isremove = false;
}

time_wheel::time_wheel(int w_size, bool isms)
    :w_size_(w_size),
     w_start_(GetNowTimeMs()),
     w_index_(0),
     time_wheel_(w_size_),
     isms_(isms)
{
    for(int i = 0; i < time_wheel_.size(); i++){
        time_wheel_[i] = new time_link;
    }
}

time_wheel::~time_wheel()
{
    for(int i = 0; i < time_wheel_.size(); i++){
        delete time_wheel_[i];
    }
}

int time_wheel::add_timeout(time_item* item)
{
    unsigned long long diff = item->ExpireTime - w_start_;

    if(!isms_)
        diff = diff / 1000;

    if(diff > w_size_){
        LOG<<"out time too big";
        return -1;
    }

    if(diff < 0){
        LOG<<"diff error!";
        return -1;
    }

    unsigned long long index = (w_index_ + diff) % w_size_;
    time_link* t_link = time_wheel_[index];
    //cout<<"add in "<<index<<endl;

    //添加节点操作
    if(t_link->tail){
        t_link->tail->next = item;
        item->prev = t_link->tail;
        item->next = NULL;
        t_link->tail = item;
    }else{
        t_link->head = item;
        t_link->tail = item;
        item->next = NULL;
        item->prev = NULL;
    }
    item->t_link = t_link;
}

void time_wheel::take_all_timeout(unsigned long long nowtime, time_link* ret_link)
{
    if(nowtime < w_start_)
        return;

    if(!ret_link){
        LOG<<"ret_link haven't init!";
        return;
    }

    int count = nowtime - w_start_;//超时个数
    if(!isms_)
        count = count / 1000 + 1;
    else
        count++;

    if(count > w_size_)
        count = w_size_;
    if(count < 0){
        LOG<<"count error!";
        return;
    }

    for(int i = 0; i < count; i++){
        int index = (w_index_ + i) % w_size_;

        //cout<<"put from "<<index<<endl;

        //将链表添加到ret链表中
        if(!time_wheel_[index]->head){
            continue;
        }
        time_item* node = time_wheel_[index]->head;
        while(node){
            node->t_link = ret_link;
            node = node->next;
        }
        node = time_wheel_[index]->head;
        if(ret_link->tail){
            node->prev = ret_link->tail;
            ret_link->tail->next = node;
            ret_link->tail = time_wheel_[index]->tail;
        }else{
            ret_link->tail = time_wheel_[index]->tail;
            ret_link->head = time_wheel_[index]->head;
        }
        time_wheel_[index]->head = NULL;
        time_wheel_[index]->tail = NULL;
    }
    w_start_ = nowtime;
    w_index_ += count-1;
}

void time_wheel::removeitem(time_item* item)
{
    time_link* link = item->t_link;

    if(item == link->head){
        link->head = item->next;
        if(link->head){
            link->head->prev = NULL;
        }
    }else if(item->prev){
        item->prev->next = item->next;
    }

    if(item == link->tail){
        link->tail = item->prev;
        if(link->tail){
            link->tail->next = NULL;
        }
    }else{
        item->next->prev = item->prev;
    }

    item->next = item->prev = NULL;
    item->t_link = NULL;
}