# Batonlib：一个高性能协程网络库

## Introduction

本项目为C++编写的协程库，并在此基础上封装了网络部分，实现了协程的切换和调度，对系统调用进行封装使开发者不用担心阻塞，实现了协程的同步和通讯。另外还实现了异步日志，记录服务器运行状态。



1. [底层协程切换的原理](https://github.com/CJLUzjj/Batonlib/blob/master/底层协程切换的原理.md)
2. [时间轮的设计](https://github.com/CJLUzjj/Batonlib/blob/master/时间轮的设计.md)
3. [协程的设计](https://github.com/CJLUzjj/Batonlib/blob/master/协程的设计.md)
4. [协程的调度](https://github.com/CJLUzjj/Batonlib/blob/master/协程的调度.md)
5. [网络库的封装](https://github.com/CJLUzjj/Batonlib/blob/master/网络库的封装.md)
6. [日志的设计](https://github.com/CJLUzjj/Batonlib/blob/master/日志的设计.md)
7. [多线程的设计]()

## Envoirment

* OS: CentOS 7.6
* Complier: g++ 4.8

## build

```
make
```

## example
协程网络库使协程透明化，使用方式与其他通用网络库类似

```
#include "net/server.h"
#include <iostream>

using namespace std;

void connected(connection* conn)
{
    cout<<"connected "<<conn<<endl;
    return;
}

void receive(connection* conn, const char* buf, size_t len)
{
    cout<<"receive data "<<conn<<endl;
    conn->send(buf, len);
    return;
}

void disconnected(connection* conn)
{
    cout<<"disconnected "<<conn<<endl;
    return;
}

int main()
{
    scheduler sch;
    server ser("0.0.0.0", 9981);
    ser.setConnectedCallBack(connected);
    ser.setReceiveCb(receive);
    ser.setDisconnectedCb(disconnected);
    ser.start();
    sch.event_loop(NULL, NULL);
}
```

## other

我是一名大三的在读学生，有问题请联系我微信pnzz849816104
