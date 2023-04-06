#include "client.hpp"
#include "server.hpp"
#include <string.h>

Client::Client(int fd, Server &server) : server(server)
{
    char hostn[128]; 
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
Client::~Client()
{
    close(this->client_fd);
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

std::string Client::get_nick_adresse(Client *temp)
{
    (void)temp;
    return (this->getNick() +  "!~" + this->getUser() + "@" + this->getHost());
}


void Client::passCmd()
{
    if (auth[0] && auth[1] &&  auth[2] && !isRegistered)
        isRegistered = true;
    if (this->commande_splited.size() < 2)
    {
        this->server.sendMsg(this->getFd(), ":IRC 461 " + (this->getNick().empty() ? "*" : this->getNick()) + " " + this->commande_splited[0] +  " :Not enough parameters\r\n");
        return ;
    }
    if (this->isRegistered)
    {
        this->server.sendMsg(this->getFd(), ":IRC 462 " + this->commande_splited[0] +  " :You may not reregister\r\n");
        return ;
    }
    if (this->commande_splited[1] != this->server.get_pass())
    {
        this->server.sendMsg(this->getFd(), ":IRC 464 " + (this->getNick().empty() ? "*" : this->getNick()) +  " :Password incorrect\r\n");
        return ;
    }
    else
    {
        this->pass_is_set = true;
        auth[0] = true;
    }
}

bool Client::checkIsRegister() {
    return isRegistered;
}

void Client::nickCmd()
{
    if (auth[0] && auth[1] &&  auth[2] && !isRegistered)
        isRegistered = true;
    if (this->commande_splited.size() < 2)
    {
        this->server.sendMsg(this->getFd(), ":IRC 461 " + (this->getNick().empty() ? "*" : this->getNick()) + " " + this->commande_splited[0] +  " :Not enough parameters\r\n");
        return ;
    }
    if (!this->pass_is_set)
    {
        this->server.sendMsg(this->getFd(), ":IRC 434 " + (this->getNick().empty() ? "*" : this->getNick()) + " :Pass is not set\r\n");
        return ;
    }
    if (this->commande_splited[1].find_first_of(" ,*?!@.") != std::string::npos ||
    this->commande_splited[1][0] == '$' || this->commande_splited[1][0] == ':' || 
    this->commande_splited[1][0] == '#')
    {
        this->server.sendMsg(this->getFd(), ":IRC 432 " + (this->getNick().empty() ? "*" : this->getNick()) + " " + this->commande_splited[1] +  " :Erroneus nickname\r\n");
        return ;
    }
    if (this->server.findNick(this->commande_splited[1]))
    {
        this->server.sendMsg(this->getFd(), ":IRC 433 " + (this->getNick().empty() ? "*" : this->getNick()) + " " + this->commande_splited[1] +  " :Nickname is already in use\r\n");
        return ;
    }
    else
    {
        this->setNick(this->commande_splited[1]);
        this->nick_is_set = true;
        auth[1] = true;
    }
   //removed sending message from server
}
void Client::userCmd()
{
    if (auth[0] && auth[1] &&  auth[2] && !isRegistered)
        isRegistered = true;
    if (this->commande_splited.size() < 5)
    {
        this->server.sendMsg(this->getFd(), ":IRC 461 " + (this->getNick().empty() ? "*" : this->getNick()) + " " + this->commande_splited[0] +  " :Not enough parameters\r\n");
        return ;
    }
    if (!this->pass_is_set)
    {
        this->server.sendMsg(this->getFd(), ":IRC 434 " + (this->getNick().empty() ? "*" : this->getNick()) + " :Pass is not set\r\n");
        return ;
    }
    if (!this->nick_is_set)
    {
        this->server.sendMsg(this->getFd(), ":IRC 434 " + (this->getNick().empty() ? "*" : this->getNick()) + " :Nick is not set\r\n");
        return ;
    }
    if (this->isRegistered)
    {
        this->server.sendMsg(this->getFd(), ":IRC 462 " + this->commande_splited[0] +  " :You may not reregister\r\n");
        return ;
    }
    this->setUser(commande_splited[1]);
    auth[2] = true;
    if (auth[0] && auth[1] && auth[2] && this->nick_is_set)
    {
        this->server.sendMsg(this->getFd(), ":IRC 001 " + this->getNick() + " :Welcome to the IRC Network, " + this->getNick() +  "[!" + this->getUser() + "@" + this->getHost() + "]" +"\r\n");
        this->isRegistered = true;
    }
}

void Client::execute()
{
    if (this->commande_splited.size() < 1)
    {
        return ;
    }
    if (this->commande_splited[0] == "PASS" || this->commande_splited[0] == "pass")
    {
        this->passCmd();
        return ;
    }
    if (this->commande_splited[0] == "USER" || this->commande_splited[0] == "user")
    {
        this->userCmd();
        return ;
    }
    if (this->commande_splited[0] == "NICK" || this->commande_splited[0] == "nick")
    {
        this->nickCmd();
        return ;
    }
} 
