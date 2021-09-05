#include"conn_obj.hpp"
// #include"thread_mgr.h"
#include"network_listen.h"
#include"http.hpp"
NetworkListen::NetworkListen()
{
    // auto t = new ThreadManager();
    // _threadMgr = t;
}
NetworkListen::~NetworkListen()
{
    // delete _threadMgr;
}
bool NetworkListen::Listen(const char* ip,int port)
{
    listenfd = CreateSocket();
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);
    // 绑定套接字（文件）和主机地址
    if(bind(listenfd,(struct sockaddr*)&server_addr,sizeof(server_addr)) == -1){
        perror("bind error");
        exit(1);
    }
    if(listen(listenfd,SOMAXCONN) == -1) {
        perror("listen error");
        exit(1);
    }
}

void NetworkListen::Accept()
{
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    while (true)
    {
        int conn = accept(listenfd,(struct sockaddr*)&client_addr,&len);
        if(conn == -1)
        {
            if(errno == EWOULDBLOCK) break; // 连接已经读完
            else if(errno == EINTR) continue; // 被信号中断
            else{}
        }
        else
        {
            int flags = fcntl(conn, F_GETFL, 0); 
            fcntl(conn, F_SETFL, flags | O_NONBLOCK);
            
            // to debug and will delete after finished
            httpResponse* httpR = new httpResponse(200,"OK","text/plain","hello, network programming");
            char* sendBuf = httpR->makePacket();
            send(conn,sendBuf,1024,0);
            delete httpR;
            delete sendBuf;


            
        }
        // ConnObj* pConn = new ConnObj(conn);
        // connDict.insert(std::make_pair(conn,pConn));
    }   
}

int NetworkListen::EpollWait(int timeout = -1)
{
    int nfds = epoll_wait(epfd,events,MAX_SIZE,timeout);
    // nfds应该只会是1
    for(int i = 0;i < nfds;i++)
    {
        if(events[i].data.fd == listenfd & EPOLLIN)
        {
            Accept();
        }
    }
    return nfds;
}