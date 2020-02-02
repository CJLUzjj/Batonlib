#ifndef TOOL_H
#define TOOL_H
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <arpa/inet.h>

unsigned long long GetNowTimeMs();

pid_t GetPid();

void SetAddr(const char* ip, const unsigned short port, struct sockaddr_in& addr);

#endif
