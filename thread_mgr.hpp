// #include<thread>
// #include<list>
#include"thread_obj.hpp"
#include<map>
#include<mutex>
#include<queue>
#include<vector>
class ThreadManager
{
private:
    /* data */
    // std::list<std::thread*>threadList;
    std::queue<int>connQueue;
    // std::map<ThreadObj*,int> _threads; 
    std::vector<ThreadObj*> _threads;
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
    number = n;
    for(int i = 0;i < n;i++)
    {
        ThreadObj* t = new ThreadObj;
        // _threads.insert(std::make_pair(t,0));
        _threads.push_back(t);
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
            // ThreadObj* t = _threads.begin()->first;
            // for(auto itr = _threads.begin();itr != _threads.end();itr++)
            // {
            //     if(_threads[itr->first] < _threads[t]) t = itr->first;
            // }
            // t->Push(conn);
            // _threads[t]++;
            // // _threads.begin()->first->Push(conn);

            _threads[conn%number]->Push(conn);
        }
        else 
        {
            break;
        }
    }
    
    
}