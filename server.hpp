#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <exception>
#include <fcntl.h>
#include <sys/poll.h>
#include <math.h>
#include <sys/time.h>

# define FAMILY AF_INET
# define TYPE SOCK_STREAM
# define   ADDR INADDR_ANY
#define TRUE             1
#define FALSE            0

class Server
{
    public:
        Server();
        Server(int port, std::string password);
        ~Server();

        class ProblemInFdServer : public std::exception
        {
            public:
                virtual const char *what() const throw();
        };
        class ProblemInBindServer : public std::exception
        {
            public:
                virtual const char *what() const throw();
        };
         class ProblemInlistenServer : public std::exception
        {
            public:
                virtual const char *what() const throw();
        };
        class ProblemInConnectServer : public std::exception
        {
            public:
                virtual const char *what() const throw();
        };
        class ProblemInSockOpt : public std::exception
        {
            public:
                virtual const char *what() const throw();
        };
        class ProblemInFcntl : public std::exception
        {
            public:
                virtual const char *what() const throw();
        };
    private:
        int fd;
        std::string password;
        int port;

};


#endif
