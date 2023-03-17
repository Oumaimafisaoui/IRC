#include "channel.hpp"

Channel::Channel(std::string name, int ownerFd)
{
    this->_name = name;
    this->_ownerFd = ownerFd;
}

std::string Channel::getChannelName()
{
    return this->_name;
}

std::map<int, std::string> Channel::getClients()
{
    return this->_clientList;
}

void Channel::addMember(int fd, std::string nickName)
{
    _clientList.insert(std::make_pair(fd, nickName));
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
    std::map<int, std::string>::iterator iterator;
    for (iterator = _clientList.begin(); iterator != _clientList.end(); iterator++)
    {
        if (send((*iterator).first, message.c_str(), message.length(), 0) == -1)
            perror ("send");
    }
}