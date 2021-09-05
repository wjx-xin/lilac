#pragma once
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<iostream>
#include<sys/epoll.h>
#include<cstring>
#include<errno.h>
#include<map>
#include "conn_obj.hpp"
#include<queue>
// #include<vector>
// class ConnObj;
class Network
{
protected:
    int epfd;
    int MAX_SIZE = 4096;
    // struct epoll_event* events;
    int listenfd; // 考虑移到NetworkListen类中
    std::map<int, ConnObj*> connDict; // socket-ConnObj
    // std::queue<int>ConnQueue;
public:
    Network(){};
    ~Network();
    struct epoll_event* events;
    int CreateSocket();
    void InitEpoll();
    void EpollAdd(int fd,uint32_t flag);
    void EpollMod(int fd,uint32_t flag);
    void EpollDel(int fd);
    virtual int EpollWait(int timeout);
};

Network::~Network()
{
    delete events;
}

int Network::CreateSocket()
{
    listenfd = socket(PF_INET,SOCK_STREAM,0);
    int flags = fcntl(listenfd, F_GETFL, 0); 
    fcntl(listenfd, F_SETFL, flags | O_NONBLOCK);
    return listenfd;
}

void Network::InitEpoll()
{
    events = new struct epoll_event[MAX_SIZE];
    epfd = epoll_create(10);
}

void Network::EpollAdd(int fd,uint32_t flag)
{
    struct epoll_event event;
    event.events = flag;
    event.data.fd = fd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event);
}

void Network::EpollMod(int fd,uint32_t flag)
{
    struct epoll_event event;
    event.events = flag;
    event.data.fd = fd;
    epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&event);
}

void Network::EpollDel(int fd)
{
    epoll_ctl(epfd,EPOLL_CTL_DEL,fd,nullptr);
}

int Network::EpollWait(int timeout = -1)
{
    int nfds = epoll_wait(epfd,events,MAX_SIZE,timeout);
    return nfds;
}