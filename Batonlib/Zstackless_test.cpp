#include "scheduler.h"
#include "coroutine_stackless.h"
#include <iostream>
using namespace std;

int main()
{
    scheduler sch;
    stackless_co a, b;
    a.user_data1 = (void*)0;
    b.user_data1 = (void*)100;

    coroutine_stackless* co1 = new coroutine_stackless(&sch, [](stackless_co* arg) ->void{
        CO_REENTER(arg->co)
        {
            cout<<"co1 step1"<<endl;

            YEILD

            cout<<"co1 step2"<<endl;

            YEILD

            cout<<"co1 step3"<<endl;
        }
    }, &a);

    coroutine_stackless* co2 = new coroutine_stackless(&sch, [](stackless_co* arg) ->void{
        CO_REENTER(arg->co)
        {
            cout<<"co2 step1"<<endl;

            YEILD

            cout<<"co2 step2"<<endl;

            YEILD

            cout<<"co2 step3"<<endl;
        }
    }, &b);

    RESUME(co1)

    RESUME(co2)

    RESUME(co1)

    RESUME(co2)

    RESUME(co1)

    RESUME(co2)

    return 0;
}