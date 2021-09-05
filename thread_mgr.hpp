// #include<thread>
// #include<list>
#include"thread_obj.hpp"
#include<map>
#include<mutex>
#include<queue>
class ThreadManager
{
private:
    /* data */
    // std::list<std::thread*>threadList;
    std::queue<int>connQueue;
    std::map<ThreadObj*,int> _threads; 
    int number; // 线程数量 
    std::mutex mtx;
public:
    ThreadManager(/* args */);
    ~ThreadManager();
    void Create(int n); // 创建线程类
    void Distribute(); // 分配

    void Push(int conn);
    int Pop();
};

ThreadManager::ThreadManager(/* args */)
{
    
}

ThreadManager::~ThreadManager()
{
}
void ThreadManager::Create(int n = 1)
{
    for(int i = 0;i < n;i++)
    {
        ThreadObj* t = new ThreadObj;
        _threads.insert(std::make_pair(t,0));
        t->Run();
        
    }
}

void ThreadManager::Push(int conn)
{
    mtx.lock();
    connQueue.push(conn);
    
    mtx.unlock();
}

int ThreadManager::Pop()
{
    int r = -1;
    mtx.lock();
    if(!connQueue.empty())
    {
        r = connQueue.front();
        connQueue.pop();
    }
    mtx.unlock();
    return r;
}

void ThreadManager::Distribute()
{
    // 验证性的，后续再改
    while(true)
    {
        int conn = Pop();
        if(conn != -1)
        {
            _threads.begin()->first->Push(conn);
        }
        else 
        {
            break;
        }
    }
    
    
}