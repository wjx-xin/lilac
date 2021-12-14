#pragma once
#include"network_listen.hpp"
#include<signal.h>
class Server
{
public:
    void Run(const char *ip,int port);
    void Run(int port);

};

void Server::Run(const char *ip,int port)
{
    signal(SIGPIPE, SIG_IGN);
    NetworkListen _net;
    int listenfd = _net.CreateSocket();   
    _net.Listen(ip,port);
    _net.InitEpoll();
    _net.EpollAdd(listenfd,EPOLLIN | EPOLLET);
    while(true)
    {
        int nfds = _net.EpollWait(100);
	//printf("server::run\n");
    }
}

void Server::Run(int port)
{
    Run("0.0.0.0",port);
}
