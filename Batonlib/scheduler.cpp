#include "scheduler.h"
#include "tool.h"
#include "coroutine.h"
#include "co_context.h"
#include "co_epoll.h"
#include "time_wheel.h"
#include "base/Logging.h"
#include <iostream>

map<pid_t, scheduler*> scheduler::g_thr_to_sch;

scheduler::scheduler()
{
    if(scheduler::g_thr_to_sch.find(GetPid()) != scheduler::g_thr_to_sch.end()){
        cout<<"scheduler error!"<<endl;
        return;
    }
    scheduler::g_thr_to_sch[GetPid()] = this;

    coroutine* self = new coroutine(this);

    self->ismain_ = 1;

    co_size_ = 1;

    context_init(&self->ctx_);

    co_call_stack_.push(self);
    //cout<<"push "<<(long long)self<<endl;

    sch_epoll_ = new co_epoll(1024);

    tw_ = new time_wheel(60*1000, true);
}

scheduler::~scheduler()
{
    g_thr_to_sch.erase(GetPid());
    while(!co_call_stack_.empty()){
        coroutine* co = co_call_stack_.top();
        co_call_stack_.pop();
        delete co;
    }
    delete sch_epoll_;
    delete tw_;
}

scheduler* scheduler::get_curr_thr_sch()
{
    if(scheduler::g_thr_to_sch.find(GetPid()) != scheduler::g_thr_to_sch.end())
        return scheduler::g_thr_to_sch[GetPid()];
    return NULL;
}

coroutine* scheduler::get_curr_thr_co()
{
    scheduler* sch = scheduler::get_curr_thr_sch();
    return sch->co_call_stack_.top();
}

int scheduler::register_event(int fd, __int32_t events, int timeout)
{
    if(timeout > tw_->get_max_size()){
        LOG<<"timeout is too big";
        return -1;
    }   

    co_epoll_item* item = NULL;
    coroutine* co  = co_call_stack_.top();

    item = new co_epoll_item;

    item->co = co;
    item->fd = fd;
    item->is_timeout = false;
    if(fd > -1)
        sch_epoll_->add(fd, events, item);//监听事件
    
    time_item* t_item;
    if(timeout > 0){
        unsigned long long now = GetNowTimeMs();

        t_item = new time_item(now + timeout, NULL, NULL);
        t_item->item = item;
        tw_->add_timeout(t_item);
        item->t_item = t_item;
    }else if(timeout == -1){
        t_item = new time_item(0, NULL, NULL);
        t_item->item = item;
        item->t_item = t_item;
    }else{
        LOG<<"timeout error";
        return -1;
    }

    //LOG<<"yield "<<(long)co;
    LOG<<"yield "<<co;
    co->yield();

    //resume回来了释放资源
    //if(!t_item->isremove)
        //tw_->removeitem(t_item);
    sch_epoll_->del(fd);
    
    bool istimeout = item->is_timeout;

    delete item;
    delete t_item;

    return istimeout;
}

void scheduler::event_loop(eventloopfunc func, void* arg)
{
    while(1){
        LOG<<"loop";
        int ret = sch_epoll_->poll(1000);
        LOG<<"epoll return, num = "<<ret;

        time_link* active = sch_epoll_->active_link_;
        time_link* timeout = sch_epoll_->timeout_link_;

        active->head = NULL;
        active->tail = NULL;

        //处理active事件
        //LOG<<"add active event";
        for(int i = 0; i < ret; i++){
            co_epoll_item* item = (co_epoll_item*)sch_epoll_->events[i].data.ptr;
            //cout<<"item: co "<<item->co<<" fd "<<item->fd<<" t_item "<<item->t_item<<endl;
            time_item* t_item = item->t_item;

            t_item->isremove = true;

            //等于0则是无超时的事件
            if(t_item->ExpireTime > 0){
                //cout<<"removeitem"<<endl;
                tw_->removeitem(t_item);
            }

            //添加事件到active
            //cout<<"add active link"<<endl;
            if(t_item->t_link){
                //cout<<"add active error"<<endl;
                LOG<<"add active error";
                return;
            }
            if(active->tail){
                //cout<<"tail not NULL"<<endl;
                active->tail->next = t_item;
                t_item->next = NULL;
                t_item->prev = active->tail;
                active->tail = t_item;
            }else{
                //cout<<"tail NULL"<<endl;
                active->head = active->tail = t_item;
                t_item->next = t_item->prev = NULL;
            }
            t_item->t_link = active;
        }

        //LOG<<"add timeout event";
        unsigned long long now = GetNowTimeMs();

        tw_->take_all_timeout(now, timeout);
        //cout<<"take all timeout return"<<endl;
        time_item* ti = timeout->head;
        while(ti){
            ti->item->is_timeout = true;
            ti = ti->next;
        }

        //cout<<"join active and timeout"<<endl;
        //将timeout和active合并
        if(timeout->head){
            time_item* ti = timeout->head;
            while(ti){
                ti->t_link = active;
                ti = ti->next;
            }
            ti = timeout->head;
            if(active->tail){
                active->tail->next = ti;
                ti->prev = active->tail;
                active->tail = timeout->tail;
            }else{
                active->head = timeout->head;
                active->tail = timeout->tail;
            }
            timeout->head = timeout->tail = NULL;
        }

        //LOG<<"do active func";
        ti = active->head;
        while(ti){

            //pop head,删除时在resume的协程中执行，这里只需要pop
            time_item* temp_item = active->head;
            if(active->head == active->tail){
                active->head = active->tail = NULL;
            }else{
                active->head = active->head->next;
            }
            temp_item->next = temp_item->prev = NULL;
            temp_item->t_link = NULL;
            if(active->head){
                active->head->prev = NULL;
            }

            //resume
            coroutine* co = ti->item->co;
            //LOG<<"resume "<<(long)co;
            LOG<<"resume "<<co;
            co->resume();
            ti = active->head;
        }

        //循环结束执行的函数
        if(func){
            if(func(arg) == -1){
                break;
            }
        }

        //处理所有执行结束的协程
        clear_co();
    }
    //cout<<"!!!!!!!"<<endl;
}

void scheduler::clear_co()
{
    while(!destroy_co_.empty()){
        coroutine* co = destroy_co_.front();
        destroy_co_.pop();
        delete co;
    }
}