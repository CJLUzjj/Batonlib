#include "tool.h"

unsigned long long GetNowTimeMs()
{
    struct timeval now = {0};
    gettimeofday(&now, nullptr);
    unsigned long long ret = now.tv_sec;
    ret *= 1000;
    ret += now.tv_usec/1000;
    return ret;
}

pid_t GetPid()
{
    static __thread pid_t pid = 0;
    static __thread pid_t tid = 0;
    if(!pid || !tid || pid != getpid() ){
        pid = getpid();
        tid = syscall( __NR_gettid );
    }
    return tid;
}

void SetAddr(const char* ip, const unsigned short port, struct sockaddr_in& addr)
{
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    int nip = 0;
    if(!ip || '\0' == *ip || 0 == strcmp(ip, "0") || 0 == strcmp(ip, "0.0.0.0") || 0 == strcmp(ip, "")){
        nip = htonl(INADDR_ANY);
    }else{
        nip = inet_addr(ip);
    }
    addr.sin_addr.s_addr = nip;
}