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
    bool getLock() {
    	int r = mtx.try_lock();
	return r;
    }
    void releaseLock(){
    	mtx.unlock();
    }
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
#include<iostream>
void ThreadObj:: WorkerLoop(ThreadObj* p)
{
	char b[1024];
    p->InitEpoll();
    while (true)
    {//printf("thread_id %ld  ,and the queue size is %d \n", std::this_thread::get_id(),p->chanel.size());
        int nfds = p->EpollWait(100);
        // printf("//=%d--\n",nfds);
        for(int i = 0;i < nfds;i++)
        {
            // ConnObj* c = p->connDict[p->events[i].data.fd];
            // c->Recv();
            // char buf[10]="hello";
            // c->Write(buf,10);
            // c->Send();
            // printf("!!**&&//=%d--\n",nfds);
            // std::cout << std::this_thread::get_id()<<std::endl;
            // sleep(2);
	    recv(p->events[i].data.fd,b,1024,0);
            //p->connDict[p->events[i].data.fd]->Recv();
            httpResponse* httpR = new httpResponse(200,"OK","text/plain","hello, network programming");
            char* sendBuf = httpR->makePacket();
            send(p->events[i].data.fd,sendBuf,1024,0);
	    //printf("hello world\n");
            delete httpR;
            close(p->events[i].data.fd);
        }
        // 下面这段是添加新的fd到epoll红黑树上
        if(!p->chanel.empty()){
	p->mtx.lock();
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
	p->mtx.unlock();
	}
        
    }
    
}

void ThreadObj::Push(int conn)
{
    //mtx.lock();
    chanel.push(conn);
   // mtx.unlock();
}

int ThreadObj::Pop()
{
    int r = -1;
    
    	if(!chanel.empty())
    	{
        	r = chanel.front();
        	chanel.pop();
    	}
    
    return r;
}

int ThreadObj::GetConnNumber() const
{
    return number;
}
