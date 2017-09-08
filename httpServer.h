#include


class httpServer
{

    public:
    int runServer();

    httpServer(short port,char* inetsddr);
    ~httpServer();
    setNonBlocking();
    isNonBlocking(){return nonblocking;}

    private:

    short port;
    char* inetaddr;
    short nonblocking;


}


