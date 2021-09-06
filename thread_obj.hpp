#include<functional>
#include<thread>
#include<mutex>
#include<atomic>
#include"network.hpp"
#include"queue"
#include"http.hpp"
class ThreadObj:public Network
{
protected:
    std::queue<int> chanel;
    std::thread* _thread;
    std::mutex mtx;
    std::atomic_int number; // 这个先不管
    std::function<void(int)> RunFunc;
public:
    ThreadObj();
    ThreadObj(std::function<void(int)> func);
    ~ThreadObj();
    static void WorkerLoop(ThreadObj* p);
    int GetConnNumber() const; // 主要返回给线程管理类,不包含chanel队列中的
    void Run();
    // chanel相关
    void Push(int conn);
    int Pop();
    int GetChanelSize();
};

ThreadObj::ThreadObj()
{
    RunFunc = [](int conn){printf("warning: need to register function in ThreadObj");};
    number = 0;
    // _thread = new std::thread(WorkerLoop,this);
}
ThreadObj::ThreadObj(std::function<void(int)> func)
{
    RunFunc = func;
    number = 0;
    // _thread = new std::thread(WorkerLoop,this);
}
ThreadObj::~ThreadObj()
{
    delete _thread;
    // 还有connDict中的连接和缓冲区需要释放
}

void ThreadObj::Run()
{
    _thread = new std::thread(WorkerLoop,this);
    _thread->detach();
}
void ThreadObj:: WorkerLoop(ThreadObj* p)
{
    p->InitEpoll();
    while (true)
    {
        int nfds = p->EpollWait(1000);
        // printf("//=%d--\n",nfds);
        for(int i = 0;i < nfds;i++)
        {
            // ConnObj* c = p->connDict[p->events[i].data.fd];
            // c->Recv();
            // char buf[10]="hello";
            // c->Write(buf,10);
            // c->Send();
            // printf("!!**&&//=%d--\n",nfds);

            p->connDict[p->events[i].data.fd]->Recv();
            httpResponse* httpR = new httpResponse(200,"OK","text/plain","hello, network programming");
            char* sendBuf = httpR->makePacket();
            send(p->events[i].data.fd,sendBuf,1024,0);
            delete httpR;
        }
        // 下面这段是添加新的fd到epoll红黑树上
        while(true)
        {
            int fd = p->Pop();
            if(fd > 0)
            {
                p->EpollAdd(fd,EPOLLIN | EPOLLET);
                ConnObj* pConn = new ConnObj(fd);
                p->connDict.insert(std::make_pair(fd,pConn));
                p->number ++;
            }
            else
            {
                break;
            }
        }
        
    }
    
}

void ThreadObj::Push(int conn)
{
    mtx.lock();
    chanel.push(conn);
    mtx.unlock();
}

int ThreadObj::Pop()
{
    int r = -1;
    mtx.lock();
    if(!chanel.empty())
    {
        r = chanel.front();
        chanel.pop();
    }
    mtx.unlock();
    return r;
}

int ThreadObj::GetConnNumber() const
{
    return number;
}