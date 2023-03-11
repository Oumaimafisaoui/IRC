#ifndef CLIENT_HPP
#define CLIENT_HPP
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
#include <vector>
#include <map>
#include <algorithm>
#include "server.hpp"

class Server;
class Client
{
    private:
        int client_fd;
        std::string nickName;
        std::string userName;
        std::string hostName;
    public:
        std::vector<std::string> commande_splited;
        Client(int fd);
        ~Client();
        std::string getNick() const;
        std::string getUser() const;
        std::string getHost() const;
        int getFd() const;
        void setFd(int fd);
        void setCommand(std::vector<std::string> command);
        void setNick(std::string nick);
        void setHost(std::string host);
        void setUser(std::string user);
}; 

#endif