# Batonlib：一个高性能协程网络库

## Introduction

本项目为C++编写的协程库，并在此基础上封装了网络部分，对系统调用进行了封装，用户不需要关心阻塞以及协程的调度。另外还实现了异步日志，记录服务器运行状态。



1. [底层协程切换的原理]()
2. [时间轮的设计]()
3. [协程的设计]()
4. [协程的调度]()
5. [网络库的封装]()
6. [日志的设计]()
7. [多线程的设计]()

## Envoirment

* OS: CentOS 7.6
* Complier: g++ 4.8

## build

```
make
```

## example

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
