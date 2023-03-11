#include "client.hpp"

Client::Client(int fd)
{
    this->client_fd = fd;
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