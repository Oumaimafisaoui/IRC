#include "channel.hpp"

Channel::Channel(std::string name, int ownerFd, std::string pass)
{
    this->_name = name;
    this->_ownerFd = ownerFd;
    _clientList.insert(ownerFd);
    this->_password = pass;
    sendToOne(ownerFd, "create channel succesful\n");
}

std::string Channel::getChannelName()
{
    return this->_name;
}

std::set<int> Channel::getClients()
{
    return this->_clientList;
}

void Channel::addMember(int fd, std::string password)
{
    if (password != this->_password)
        sendToOne(fd, "Permission denied Please put a good password\n");
    else if (isMember(fd))
        sendToOne(fd, "You've already joined this channel\n");
    else
    {
        _clientList.insert(fd);
        sendToMembers("New use just joined this channel\n");
    }
}

void Channel::removeMember(int fd)
{
    _clientList.erase(fd);
}

bool Channel::isMember(int fd)
{
    return _clientList.find(fd) != _clientList.end();
}

void Channel::sendToMembers(std::string message)
{
    std::set<int>::iterator iterator;
    for (iterator = _clientList.begin(); iterator != _clientList.end(); iterator++)
    {
        if (send((*iterator), message.c_str(), message.length(), 0) == -1)
            perror ("send");
    }
}

void Channel::sendToOne(int fd, std::string message)
{
    if (send(fd, message.c_str(), message.length(), 0) == -1)
        perror ("send");
}