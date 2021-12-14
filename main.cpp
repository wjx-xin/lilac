#include"server.hpp"

int main()
{
    Server s;
    // s.Run("127.0.0.1",8888);
    s.Run(80);

    return 0;
}
