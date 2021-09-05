#include "buffer.hpp"

class ConnObj
{
private:
    int _socket;
    const int RW_SIZE = 1024;
    // char ;
    // int connfd;
    Buffer *recvBuf;
    Buffer *sendBuf;
public:
    ConnObj(){}
    ConnObj(int s):_socket(s){}
	int GetSocket() const { return _socket; }
	void Recv();
	void Send();
    void Write(char* data,int size); // 向发送缓冲区写数据

    int RecvBufLength() const{return recvBuf->GetLength();}
	int SendBufLength() const{return sendBuf->GetLength();}
};

void ConnObj::Recv()
{
    char* buf = new char[RW_SIZE];
    while (true)
    {
        int len = recv(_socket,buf,RW_SIZE,0);
        if(len == -1 || len == 0) break; // 后期可以根据错误码，分类处理
        recvBuf->Save(buf,len);
        memset(buf,0,RW_SIZE);
    }
    delete []buf;
}

void ConnObj::Send()
{
    char* buf = new char[RW_SIZE];
    int begin = 0;
    while(true)
    {
        int size = 0;
        sendBuf->Read(buf,RW_SIZE,&size);
        if(size == 0) break;
        while (true)
        {
            // 把buf里的数据发出去
            int len = send(_socket,buf+begin,size,0);
            if(len == size) break;
            if(len == -1) break; // 出错，后续再处理，这里还是很重要的，不能这么随便
            if(len < size) begin += len; // 发出去len个，起始位置偏移到begin+len
            size = size - len;
        }
        memset(buf,0,RW_SIZE);
    }
}

void ConnObj::Write(char* data,int size)
{
    sendBuf->Save(data,size);
}