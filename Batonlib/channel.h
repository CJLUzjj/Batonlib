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
    bool is_yield_;
    bool close_;

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

    bool put(T val);
    bool get(T& val);
    bool isfull(){return cap_ == size_;}
    bool isempty(){return size_ == 0;}
    bool isyield(){return is_yield_;}
    void close();
};

template<class T>
channel<T>::channel()
    :sch_(scheduler::get_curr_thr_sch()),
    cap_(1),
    size_(0),
    empty_(new co_condition(sch_)),
    full_(new co_condition(sch_)),
    is_yield_(false),
    close_(false)
{

}

template<class T>
channel<T>::channel(scheduler* sch)
    :sch_(sch),
    cap_(1),
    size_(0),
    empty_(new co_condition(sch_)),
    full_(new co_condition(sch_)),
    is_yield_(false),
    close_(false)
{

}

template<class T>
channel<T>::channel(int cap)
    :sch_(scheduler::get_curr_thr_sch()),
    cap_(cap),
    size_(0),
    empty_(new co_condition(sch_)),
    full_(new co_condition(sch_)),
    is_yield_(false),
    close_(false)
{

}

template<class T>
channel<T>::channel(scheduler* sch, int cap)
    :sch_(sch),
    cap_(cap),
    size_(0),
    empty_(new co_condition(sch_)),
    full_(new co_condition(sch_)),
    is_yield_(false),
    close_(false)
{

}

template<class T>
channel<T>::~channel()
{
    delete empty_;
    delete full_;
}

template<class T>
bool channel<T>::put(T val)
{
    if(close_)return false;

    while(size_ == cap_){
        is_yield_ = true;
        full_->co_cond_wait();
        if(close_)
            return false;
    }

    is_yield_ = false;

    queue_.push(val);

    size_++;
    
    empty_->co_cond_signal();

    return true;
}

template<class T>
bool channel<T>::get(T& val)
{
    if(close_)return false;
    
    while(size_ == 0){
        is_yield_ = true;
        empty_->co_cond_wait();
        if(close_)
            return false;
    }

    is_yield_ = false;

    //这里或许可以用std::move来优化拷贝
    T temp = queue_.front();
    val = temp;
    queue_.pop();
    size_--;

    full_->co_cond_signal();

    return true;
}

template<class T>
void channel<T>::close()
{
    close_ = true;
    full_->co_cond_broadcast();
    empty_->co_cond_broadcast();
    return;
}



#endif