#include "scheduler.h"
#include <iostream>
#include "coroutine_local.h"

using namespace std;

COROUTINE_LOCAL(std::vector<int>, co_local)

int main()
{
    scheduler sch;
    for(int i = 0; i < 10; i++){
        coroutine* co = new coroutine(&sch, 1024*1024, [i](void* arg) ->void*{
            COLOCAL(co_local).push_back(i);
            cout<<"coroutine "<<coroutine::get_curr_co()<<":"<<COLOCAL(co_local)[0]<<endl;
            DELCOLOCAL(co_local, coroutine::get_curr_co());
        }
        , nullptr);
        co->resume();
    }
    return 0;
    sch.event_loop(NULL, NULL);
}