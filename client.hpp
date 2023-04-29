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
        int error_pss;
        int error_usr;
        int error_nck;
        std::string nickName;
        std::string userName;
        std::string hostName;
        bool        isRegistered;
        bool        pass_is_set;
        bool        nick_is_set;
        bool        auth[3];
        friend class Server;
        bool is_operator;
        Server &server;
    public:
        std::vector<std::string> commande_splited;
        Client(int fd, Server &server);
        ~Client();
        std::string getNick() const;
        std::string getUser() const;
        std::string getHost() const;
        bool get_isoperator() const;
        void setOperatorStatus(bool oper);
        int getFd() const;
        bool checkIsRegister();
        void setFd(int fd);
        void setCommand(std::vector<std::string> command);
        void setNick(std::string nick);
        void setHost(std::string host);
        void setUser(std::string user);
        int execute();
        int passCmd();
        int userCmd();
        int nickCmd();
        std::string get_nick_adresse();
}; 

#endif
