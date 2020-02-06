#ifndef CHANNEL_H
#define CHANNEL_H
#include <queue>
#include "co_condition.h"
#include "scheduler.h"
#include "base/Mutex.h"
using namespace std;

template <class T>
class channel
{
private:
    scheduler* sch_;

    const int cap_;
    int size_;

    co_condition* empty_;
    co_condition* full_;

    queue<T> queue_;

    //Mutex mutex_;
public:
    channel();
    channel(scheduler* sch);
    channel(int cap);
    channel(scheduler* sch, int cap);
    ~channel();

    void put(T val);
    T get();
    bool isfull(){return cap_ == size_;}
    bool isempty(){return size_ == 0;}
};

template<class T>
channel<T>::channel()
    :sch_(scheduler::get_curr_thr_sch()),
    cap_(1),
    size_(0),
    empty_(new co_condition(sch_)),
    full_(new co_condition(sch_)),
{

}

template<class T>
channel<T>::channel(scheduler* sch)
    :sch_(sch),
    cap_(1),
    size_(0),
    empty_(new co_condition(sch_)),
    full_(new co_condition(sch_)),
{

}

template<class T>
channel<T>::channel(int cap)
    :sch_(scheduler::get_curr_thr_sch()),
    cap_(cap),
    size_(0),
    empty_(new co_condition(sch_)),
    full_(new co_condition(sch_)),
{

}

template<class T>
channel<T>::channel(scheduler* sch, int cap)
    :sch_(sch),
    cap_(cap),
    size_(0),
    empty_(new co_condition(sch_)),
    full_(new co_condition(sch_)),
{

}

template<class T>
channel<T>::~channel()
{
    delete empty_;
    delete full_;
}

template<class T>
void channel<T>::put(T val)
{
    while(size_ == cap_){
        full_->co_cond_wait();
    }

    queue_.push(val);

    size_++;
    if(size_ == 1){
        empty_->co_cond_signal();
    }
}

template<class T>
T channel<T>::get()
{
    while(size_ == 0){
        empty_->co_cond_wait();
    }

    //这里或许可以用std::move来优化拷贝
    T ret = queue_.front();
    queue_.pop();
    size_--;

    if(size_ == cap_ - 1){
        full_->co_cond_signal();
    }

    return ret;
}

#endif