#include "net/server.h"
#include "scheduler.h"
#include <functional>
#include <iostream>
#include <string>
#include <map>

using namespace std;

struct client_inf{
    int id_;
    connection* conn_;
};

class chat_server
{
private:
    int client_size_;
    vector<bool> used_id_;
    map<connection*, client_inf*> m_;
public:
    chat_server(int size);
    ~chat_server();

    void connected(connection* conn);

    void receive(connection* conn, const char* buf, size_t len);

    void disconnected(connection* conn);
};

chat_server::chat_server(int size)
    :client_size_(size)
{
    used_id_ = vector<bool>(client_size_, false);
}

chat_server::~chat_server()
{
}

void chat_server::connected(connection* conn)
{
    conn->set_read_timeout(50*1000);
    conn->set_write_timeout(10*1000);

    int id = -1;
    for(int i = 0; i < used_id_.size(); i++){
        if(!used_id_[i]){
            id = i;
            used_id_[i] = true;
            break;
        }
    }
    if(id == -1){
        conn->close();
        return;
    }

    client_inf* inf = new client_inf;
    inf->conn_ = conn;
    inf->id_ = id;

    m_[conn] = inf;

    string out;
    out += "connect success your id is " + to_string(id) + "\r\n";
    conn->send(out.c_str(), out.size());
}

void chat_server::receive(connection* conn, const char* buf, size_t len)
{
    string out;
    out += "id:" + to_string(m_[conn]->id_) + " say:";
    out += string(buf, len) + "\r\n";
    for(auto it = m_.begin(); it != m_.end(); it++){
        it->first->send(out.c_str(), out.size());
    }
}

void chat_server::disconnected(connection* conn)
{
    client_inf* inf = m_[conn];
    m_.erase(conn);
    used_id_[inf->id_] = false;
    delete inf;
    string out;
    if(conn->connection_statue_ == conn->close_by_server_other){
        out += "close by server\r\n";
    }else if(conn->connection_statue_ == conn->close_by_server_timeout){
        out += "close by server because of time out\r\n";
    }
    if(!out.empty()){
        conn->send(out.c_str(), out.size());
    }
}


int main()
{
    scheduler sch;
    server ser("0.0.0.0", 9981);
    chat_server chat_ser(5);
    ser.setConnectedCb(bind(&chat_server::connected, &chat_ser, placeholders::_1));
    ser.setReceiveCb(bind(&chat_server::receive, &chat_ser, placeholders::_1, placeholders::_2, placeholders::_3));
    ser.setDisconnectedCb(bind(&chat_server::disconnected, &chat_ser, placeholders::_1));
    ser.start();
    sch.event_loop(NULL, NULL);
}