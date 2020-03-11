#pragma once

#include <map>
#include <thread>

class coroutine;

#define COROUTINE_LOCAL(name, y) \ 
thread_local map<coroutine* ,name> y;



#define COLOCAL(y) \
y[coroutine::get_curr_co()]

#define DELCOLOCAL(y, coptr) \
if(y.find(coptr) != y.end()){ \
    y.erase(coptr); \
}