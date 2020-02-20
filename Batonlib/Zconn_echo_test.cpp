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
    ser.setConnectedCb(connected);
    ser.setReceiveCb(receive);
    ser.setDisconnectedCb(disconnected);
    ser.start();
    sch.event_loop(NULL, NULL);
}