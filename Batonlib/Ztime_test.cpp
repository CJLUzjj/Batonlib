#include "time_wheel.h"
#include "tool.h"
#include <iostream>
#include <unistd.h>

using namespace std;

void time_func(void* arg)
{
    int* i = (int*)arg;
    cout<<"I'am No."<<*i<<endl;
}

int main()
{
    time_wheel tw(60*1000, true);

    unsigned long long now = GetNowTimeMs();
    timeout_func func = time_func;
    
    for(int i = 0; i < 60; i++){
        auto t = now + i*1000;
        int* int_arg = new int;
        *int_arg = i*1000;
        time_item* ti = new time_item(t, func, (void*)int_arg);
        //这里有内存泄漏问题
        tw.add_timeout(ti);
    }
    
    while(1){
        sleep(1);
        cout<<"one second..."<<endl;
        time_link* tl = new time_link;
        tw.take_all_timeout(GetNowTimeMs(), tl);
        time_item* node = tl->head;
        while(node){
            node->process(node->arg);
            node = node->next;
        }
    }

    return 0;
}