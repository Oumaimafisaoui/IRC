#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

# define FAMILY AF_INET
# define TYPE SOCK_STREAM
# define   ADDR INADDR_ANY
class Server
{
    public:
        Server();
        Server(int port, std::string password);
        ~Server();
    private:
        int fd;
        std::string password;
        int port;

};


#endif
