#ifndef TIME_WHEEL_H
#define TIME_WHEEL_H
#include <functional>
#include <vector>
using namespace std;

typedef function<void(void*)> timeout_func;

struct time_item;

struct time_link
{
    time_item* head;
    time_item* tail;

    time_link()
    {
        head = NULL;
        tail = NULL;
    }
};

struct co_epoll_item;

struct time_item
{
    time_item* next;
    time_item* prev;

    time_link* t_link;

    unsigned long long ExpireTime;

    timeout_func process;

    void* arg;

    co_epoll_item* item;
    bool isremove;

    time_item(unsigned long long ex_t, timeout_func func, void* func_arg)
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
};


class time_wheel
{
private:
    int w_size_;

    //时间轮最近一次处理超时事件的时间，即该时间之后到当前时间中间的都是下一次超时事件
    unsigned long long w_start_;
    //w_start_所在时间的下标
    unsigned long long w_index_;

    vector<time_link*> time_wheel_;

    bool isms_;
public:
    time_wheel(int w_size, bool isms);//默认最长超时时间为60秒
    ~time_wheel();
    
    int add_timeout(time_item* item);
    void take_all_timeout(unsigned long long nowtime, time_link* ret_link);
    void removeitem(time_item* item);

    int get_max_size()
    {
        return w_size_;
    }
};
#endif