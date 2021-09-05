#include<cstring>
class Buffer
{
private:
    char* _buffer;
    int bufferSize;
    int dataLength; //dataLength = (end+bufferSize-begin+1)%bufferSize
    int begin;
    int end; // end == begin-1时，可能为空，也可能满
private:
    void copy(char* dest,int size); // copy to dest
    void ReMalloc(int size); // 
public:
    // void get(){ //调试用
    //     printf("begin-%d end-%d len-%d bufsize-%d\n",begin,end,dataLength,bufferSize);
    //     for(int i = begin;i < begin+dataLength;i++) 
    //         printf("%c",_buffer[i]);
    //     printf("\n");
    // }
    Buffer(){}
    Buffer(int size);
    ~Buffer();
    void Save(char* data,int size);
    void Read(char* buf,int size,int* realSize);
    int GetLength() const {return dataLength;} //获取数据长度
    
};
Buffer::Buffer(int size):bufferSize(size),begin(0),end(-1),dataLength(0)
{
    _buffer = new char[size];
}
Buffer::~Buffer()
{
    delete []_buffer;
}
void Buffer::copy(char* dest,int size)
{
    if(size > dataLength) return;
    int endIndex = (begin+size-1)%bufferSize;
    if(begin < endIndex)
    {
        memcpy(dest,_buffer+begin,size);
    }
    else if(begin > endIndex)
    {
        memcpy(dest,_buffer+begin,bufferSize-begin);
        memcpy(dest+bufferSize-begin,_buffer,endIndex);
    }
    // 不会出现begin == endIndex的情况
}

void Buffer::Save(char* data,int size)
{
    // 需要保证size是data的长度，否则会出错
    if(size > bufferSize - dataLength) ReMalloc(size); // endindex不可能和begin相等

    int endIndex = (end + size) % bufferSize; // 填充后的终止位置

    if(endIndex > end) 
    {
        // 填充后在[begin,bufferSize)之间
        memcpy(_buffer + end + 1,data,size);
    }
    else // 将要填充的数据跨过了数组末端
    {
        memcpy(_buffer+end+1,data,bufferSize-end-1); // _buffer+end+1,end后一个开始填充
        memcpy(_buffer,data+bufferSize-end-1,endIndex+1);
    }

    end = (end + size) % bufferSize;
    dataLength = (end + bufferSize - begin + 1) % bufferSize;
}

void Buffer::Read(char* buf,int size,int* realSize)
{
    // 返回值--buf，返回值--realSize实际返回的buf长度
    if(dataLength == 0) 
    {
        *realSize = 0;
        return;
    }
    if(size >= dataLength)
    {
        copy(buf,dataLength);
        *realSize = dataLength;
        begin = (begin + size) % bufferSize;
        dataLength = 0;
    }
    else
    {
        copy(buf,size);
        *realSize = size;
        begin = (begin + size) % bufferSize;
        dataLength -= size;
    }
}

void Buffer::ReMalloc(int size)
{
    char* newbuf = nullptr;
    if(bufferSize < 1024*1024)
    {
        newbuf = new char[bufferSize*2+size];
        bufferSize *= 2 + size;
    }
    else
    {
        newbuf = new char[bufferSize+2048+size];
        bufferSize = bufferSize + 2048 + size;
    }
    
    copy(newbuf,dataLength);
    begin = 0;
    end = dataLength-1; // 会出现-1
    char* temp = _buffer;
    _buffer = newbuf;
    delete []temp;
}

// void Buffer::copy(char* dest)
// {
//     if(begin < end)
//         memcpy(dest,_buffer+begin,dataLength);
//     else if(begin > end)
//     {
//         memcpy(dest,_buffer+begin,bufferSize-begin);
//         memcpy(dest+bufferSize-begin,_buffer,end);
//     }
//      
// }