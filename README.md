一、网络模型
基于Reactor模型，以及所谓的one thread one loop思想。使用Epoll函数，EPOLLET模式+非阻塞IO实现。

二、主要逻辑
主线程的MainReactor，监听listenfd，创建新的TCP连接，而后把文件描述符发送给_threadManager，由_threadManager分发给各个工作线程（A B C...)。工作线程把接收到的文件描述符添加到自身的Epoll红黑树上，负责处理该连接相关的IO操作。

目前初步具备了雏形，完善中...

三、主要文件
buffer.hpp    main.cpp            README.md
conn_obj.hpp  network.hpp         server.hpp
http.hpp      network_listen.hpp  thread_mgr.hpp
LICENSE       NOTE.md             thread_obj.hpp

四、使用示例
#include"server.hpp"

int main()
{
    Server s;
    s.Run("127.0.0.1",8888);
    return 0;
}

g++ main.cpp -o main -lpthread && ./main
