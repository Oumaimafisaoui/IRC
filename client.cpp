#include "client.hpp"
#include "server.hpp"
#include <string.h>

Client::Client(int fd, Server &server) : server(server)
{
    char hostn[128]; 
    is_operator = false;
    error_pss = 0;
    error_usr = 0;
    error_nck = 0;
    if (gethostname(hostn, 128) == -1)
    {
        perror("gethostname");
    }
    this->client_fd = fd;
    this->isRegistered = false;
    this->pass_is_set = false;
    nickName = "";
    userName = "";
    hostName = std::string(hostn);
    buff_client = "";
    memset(auth, false, sizeof(bool) * 3);
}

void Client::setOperatorStatus(bool oper)
{
    this->is_operator = oper;
}

bool Client::get_isoperator() const
{
    return (this->is_operator);
}
Client::~Client()
{
    if (this->client_fd != -1) {
        shutdown(this->client_fd, SHUT_WR); // shutdown socket for writing
        char buf[4096];
        while (recv(this->client_fd, buf, 4096, 0) > 0) {} // read and discard any remaining data
        close(this->client_fd);
    }
}

std::string Client::getNick() const
{
    return (this->nickName);
}
std::string Client::getUser() const
{
    return (this->userName);
}
std::string Client::getHost() const
{
    return (this->hostName);
}
int Client::getFd() const
{
    return (this->client_fd);
}
void Client::setNick(std::string nick)
{
    this->nickName = nick;
}
void Client::setHost(std::string host)
{
    this->hostName = host;
}
void Client::setUser(std::string user)
{
    this->userName = user;
}
void Client::setFd(int fd)
{
    this->client_fd = fd;
}
void Client::setCommand(std::vector<std::string> command)
{
    this->commande_splited = command;
}

std::string Client::get_nick_adresse()
{
    return (this->getNick() +  "!~" + this->getUser() + "@" + this->getHost());
}


int Client::passCmd()
{
    if (this->commande_splited.size() < 2)
    {
        this->server.sendMsg(this->getFd(), ":localhost 461 " + (this->getNick().empty() ? "*" : this->getNick()) + " " + this->commande_splited[0] +  " :Not enough parameters\r\n");
        this->error_pss = 1;
        return 0;
    }
    if (this->isRegistered)
    {
        this->server.sendMsg(this->getFd(), ":localhost 462 " + this->commande_splited[0] +  " :You may not reregister\r\n");
        this->error_pss = 1;
        return 0;
    }
    if (this->commande_splited[1] != this->server.get_pass())
    {
        this->server.sendMsg(this->getFd(), ":localhost 464 " + (this->getNick().empty() ? "*" : this->getNick()) +  " :Password incorrect\r\n");
        this->error_pss = 1;
        return 0;
    }
    else
    {
        this->pass_is_set = true;
        auth[0] = true;
        this->error_pss = 0;
        return (1);
    }
    return (0);
}

bool Client::checkIsRegister() {
    return isRegistered;
}

int Client::nickCmd()
{

        if (this->commande_splited.size() < 2)
        {
            this->server.sendMsg(this->getFd(), ":localhost 431 " + (this->getNick().empty() ? "*" : this->getNick()) + " " + this->commande_splited[0] +  " :No nickname given\r\n");
            this->error_nck = 1;
            return 0;
        }
        else if (this->commande_splited[1].find_first_of(" ,*?!@.") != std::string::npos ||
        this->commande_splited[1][0] == '$' || this->commande_splited[1][0] == ':' || 
        this->commande_splited[1][0] == '#' || this->commande_splited[1].size() > 9 || this->commande_splited[1] == "nick" ||this->commande_splited[1] == "NICK")
        {
            this->server.sendMsg(this->getFd(), ":localhost 432 " + (this->getNick().empty() ? "*" : this->getNick()) + " " + this->commande_splited[1] +  " :Erroneus nickname\r\n");
                this->error_nck = 1;
            return 0;
        }
        else if (this->server.findNick(this->commande_splited[1]))
        {
            this->server.sendMsg(this->getFd(), ":localhost 433 " + (this->getNick().empty() ? "*" : this->getNick()) + " " + this->commande_splited[1] +  " :Nickname is already in use\r\n");
            this->error_nck = 1;
            return 0;
        }
        else if(error_nck == 0)
        {
                this->setNick(this->commande_splited[1]);
                this->nick_is_set = true;
                auth[1] = true;
                this->error_nck = 0;
                return (1);
        }
    return (0);
   //removed sending message from server
}
int Client::userCmd()
{
        if (this->commande_splited.size() < 5)
        {
            this->server.sendMsg(this->getFd(), ":localhost 461 " + (this->getNick().empty() ? "*" : this->getNick()) + " " + this->commande_splited[0] +  " :Not enough parameters\r\n");
            return 0;
        }
        else if (this->isRegistered)
        {
            this->server.sendMsg(this->getFd(), ":localhost 462 " + this->commande_splited[0] +  " :You may not reregister\r\n");
            return 0;
        }
        else
        {
            if(pass_is_set == 1 && nick_is_set == 1)
            {
                this->setUser(commande_splited[1]);
                auth[2] = true;
                if (auth[0] && auth[1] && auth[2])
                {
                    this->server.sendMsg(this->getFd(), ":localhost 001 " + this->getNick() + " :Welcome to the localhost Network, " + this->getNick() +  "[!" + this->getUser() + "@" + this->getHost() + "]" +"\r\n");
                    this->isRegistered = true;
                    return (1);
                } 
            }
            else {

                if (!this->pass_is_set)
                {
                    this->server.sendMsg(this->getFd(), ":localhost 434 " + (this->getNick().empty() ? "*" : this->getNick()) + " :Pass is not set\r\n");
                    return 0;
                }
                if (!this->nick_is_set)
                {
                    this->server.sendMsg(this->getFd(), ":localhost 434 " + (this->getNick().empty() ? "*" : this->getNick()) + " :Nick is not set\r\n");
                    return 0;
                }
            }
        }
    return (0);
}

int Client::execute()
{
    if (this->commande_splited[0] == "PASS" || this->commande_splited[0] == "pass")
    {
        if(this->passCmd())
            return 1;
        return (0);
    }
    if (this->commande_splited[0] == "USER" || this->commande_splited[0] == "user")
    {
        if(this->userCmd())
            return 1;
        return (0);
    }
    if (this->commande_splited[0] == "NICK" || this->commande_splited[0] == "nick")
    {
        if(this->nickCmd())
            return 1;
        return (0);
    }
    return (1);
} 
