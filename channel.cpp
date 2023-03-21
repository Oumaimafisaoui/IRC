#include "channel.hpp"

Channel::Channel(std::string name, Client *_client, std::string pass)
{
    this->_name = name;
    this->_owner = _client;
    _clientList.insert(_client);
    this->_password = pass;
    sendToOne(_client->getFd(), "create channel succesful\n");
}

std::string Channel::getChannelName()
{
    return this->_name;
}

std::set<Client *> Channel::getClients()
{
    return this->_clientList;
}

void Channel::addMember(Client *_client, std::string password)
{
    if (password != this->_password && password != "")
        sendToOne(_client->getFd(), "Permission denied Please put a good password\n");
    else if (isMember(_client))
        sendToOne(_client->getFd(), "You've already joined this channel\n");
    else
    {
        _clientList.insert(_client);
        sendToMembers("New use just joined this channel\n");
    }
}

void Channel::removeMember(Client *_client)
{
    _clientList.erase(_client);
}

bool Channel::isMember(Client *_client)
{
    return _clientList.find(_client) != _clientList.end();
}

void Channel::sendToMembers(std::string message)
{
    std::set<Client *>::iterator iterator;
    for (iterator = _clientList.begin(); iterator != _clientList.end(); iterator++)
    {
        if (send((*iterator)->getFd(), message.c_str(), message.length(), 0) == -1)
            perror ("send");
    }
}

void Channel::sendToOne(int fd, std::string message)
{
    if (send(fd, message.c_str(), message.length(), 0) == -1)
        perror ("send");
}