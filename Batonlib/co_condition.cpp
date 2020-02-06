#include "co_condition.h"

co_condition::co_condition()
{
    sch_ = scheduler::get_curr_thr_sch();
}

co_condition::co_condition(scheduler* sch)
    :sch_(sch)
{

}

co_condition::~co_condition()
{
}

int co_condition::co_cond_wait()
{
    co_epoll_item* item = new co_epoll_item();
    item->fd = -1;
    item->co = scheduler::get_curr_thr_co();
    item->t_item = NULL;
    item->is_timeout = false;

    time_item* t_item = new time_item(0, NULL, NULL);

    item->t_item = t_item;
    t_item->item = item;

    co_deque_.push_back(item);

    item->co->yield();

    //resume
    delete item;
    delete t_item;

    return 1;
}

int co_condition::co_cond_signal()
{
    if(co_deque_.empty())
        return -1;
    time_item* item = co_deque_.front()->t_item;
    co_deque_.pop_front();

    time_link* active = sch_->sch_epoll_->active_link_;

    //添加到活跃事件中
    if(active->tail){
        active->tail->next = item;
        item->next = NULL;
        item->prev = active->tail;
        active->tail = item;
    }else{
        active->head = active->tail = item;
        item->next = item->prev = NULL;
    }
    item->t_link = active;

    return 1;
}

int co_condition::co_cond_broadcast()
{
    while(!co_deque_.empty()){
        time_item* item = co_deque_.front()->t_item;
        co_deque_.pop_front();

        time_link* active = sch_->sch_epoll_->active_link_;

        //添加到活跃事件中
        if(active->tail){
            active->tail->next = item;
            item->next = NULL;
            item->prev = active->tail;
            active->tail = item;
        }else{
            active->head = active->tail = item;
            item->next = item->prev = NULL;
        }
        item->t_link = active;
    }

    return 1;
}