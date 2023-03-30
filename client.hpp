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

class Server;

class Client
{
    private:
        std::string buff_client;
        int client_fd;
        std::string nickName;
        std::string userName;
        std::string hostName;
        bool        isRegistered;
        bool        pass_is_set;
        bool        nick_is_set;
        bool        auth[3];
        friend class Server;
        Server &server;
    public:
        std::vector<std::string> commande_splited;
        Client(int fd, Server &server);
        ~Client();
        std::string getNick() const;
        std::string getUser() const;
        std::string getHost() const;
        int getFd() const;
        bool checkIsRegister();
        void setFd(int fd);
        void setCommand(std::vector<std::string> command);
        void setNick(std::string nick);
        void setHost(std::string host);
        void setUser(std::string user);
        void execute();
        void passCmd();
        void userCmd();
        void nickCmd();
        std::string get_nick_adresse(Client * tmp);
}; 

#endif
