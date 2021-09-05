使用示例
#include"server.hpp"

int main()
{
    Server s;
    s.Run("127.0.0.1",8888);
    return 0;
}

g++ main.cpp -o main -lpthread && ./main

目前初步具备了雏形，完善中...