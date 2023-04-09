#ifndef SERVER_HPP
#define SERVER_HPP
#include <sstream>
#include <netinet/in.h>
#include <utility>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <sys/socket.h>
#include <sys/un.h> 
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
#include "client.hpp"
#include <numeric>
#include "channel.hpp"
// #include <sys/un.h>
// #include <curl/curl.h>


# define FAMILY AF_INET
# define TYPE SOCK_STREAM
# define   ADDR INADDR_ANY
#define TRUE             1
#define FALSE            0
#define MAX 200

class Client;
class Server
{
    public:
        Server();
        Server(int port, std::string password);
        ~Server();
        void launch_socket(void);

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
        class ProblemInPoll : public std::exception
        {
            public:
                virtual const char *what() const throw();
        };
    
        Client *find_client(const std::string &nick) const;
        Channel *find_channel(const std::string &name) const;
        std::string get_pass() const;
        int get_port() const;
        int get_fd() const;
        void receive_message(std::vector<pollfd>::iterator i, Client *client, int len);
        void client_not_connected(Client *client);
        void client_connected(Client *client);
        void sendMsg(int fd, std::string msg);
        bool findNick(std::string &nick);
        void printAllClients();
        Channel *_findChannel(std::string name);
        Client *findClientByNick(std::string name);
        std::vector<std::string> joinCmdParser(std::string params);
        std::map<int, Client> getClients();
    private:
       int fd;
       bool is_active;
        std::string password;
        int port;
       std::vector<pollfd> poll_vec;
       bool off;
       std::map<int, Client*> clients;
       std::vector<Channel *> _channels;
       char buffer[500];
       std::string message;
       std::vector<std::string> _command_splited;
       Client *client;
       bool _isNotChannelCmd(std::vector<std::string> command_splited);
       void _execute_commands(Client *clien);
       void _joinCmd(Client *client);
       void _modeCmd(Client *client);
       void _privMsgCmd(Client *client, bool error);
       void _topicCmd(Client *client);
       void _inviteCmd(Client *client);
       void _botCmd(Client *client);     
        void _botQuote(Client *client);
       void _partCmd(Client *client);
       void _kickCmd(Client *client);
       void _quitCmd(Client *client);
       void _freeAll();
       std::string getmessage(Client *client, std::string &commands, size_t dots);
        void sendmessage(std::string &message, Client* client, std::string &commands, size_t dots, bool error);
       void find_client_and_sendmsg1(Client *client, std::string &target, std::string &message, bool error);
       void find_channel_and_sendmsg1(Client *client, std::string &target, std::string &message, bool error);
};


#endif
