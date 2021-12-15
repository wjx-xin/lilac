#pragma once
#include<thread>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<atomic>
class Task
{
public:
    using ptr = std::shared_ptr<Task>;
    virtual void runTask(){};
};

class ThreadPool
{
private:
    std::queue<Task::ptr>que;
    std::mutex mu;
    std::condition_variable cond;
    std::vector<std::thread>threads;
    std::atomic<int> free {0};
    std::mutex mtx;
    int size;
public:
    bool stop {false};
    ThreadPool(){};
    ThreadPool(int size);
    ~ThreadPool();
    static void workLoop(ThreadPool* p);
    bool push(Task::ptr);
    void cancel() {stop = true;}
};

ThreadPool::ThreadPool(int size):size(size)
{
    threads = std::vector<std::thread>(size);
    for(int i = 0;i < size;i++)
    {
        threads[i] = std::thread(workLoop,this);
    }
}
ThreadPool::~ThreadPool()
{
    cond.notify_all();
    stop = true;
    for(auto &t : threads)
    {
        t.join();
    }
}
bool ThreadPool::push(Task::ptr task)
{
    mu.lock();
    que.push(task);
    mu.unlock();
    cond.notify_one();
    return true;
}

void ThreadPool::workLoop(ThreadPool* p)
{
    while(true)
    {
        std::unique_lock<std::mutex> lock(p->mu);
        p->cond.wait(lock,[&p](){
            return (p->stop) || !(p->que).empty();
        });
        if(p->stop && (p->que).empty()) return;
        auto task = p->que.front();
        (p->que).pop();
        lock.unlock();

        task->runTask();
    }
}
