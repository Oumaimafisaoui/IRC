#include "channel.hpp"

Channel::Channel(std::string name, Client *_client, std::string password)
{
    this->_name = name;
    this->_owner = _client;
    this->_operators.insert(_client->getNick());
    _clientList.insert(_client);
    this->_password = password;
    this->_topic = "";
    sendToOne(_client->getFd(), this->_name + " channel succesful created\n");
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
    std::cout << " addMember" << std::endl;
    if (checkModes("+i"))
    {
        if (invitedLists.find(_client->getNick()) == invitedLists.end())
        {
            sendToOne(_client->getFd(), _client->getNick() + " " + _name +  " :" + "Cannot join channel (+i)\n" );
            return ;
        }
    }
    if (password != this->_password && this->_password != "")
        sendToOne(_client->getFd(), _client->getNick() + " " + _name +  " :" + "Cannot join channel (+k)\n" );
    else if (isMember(_client))
        sendToOne(_client->getFd(), "You've already joined this channel\n");
    else
    {
        _clientList.insert(_client);
        sendToOne(_client->getFd(), _client->getNick() + " :just joined the Channel " + this->_name + "\n");
        if (_topic != "")
            sendToOne(_client->getFd(), _client->getNick() + " " + this->_name + " :" + this->_topic + "\n");
    }
    std::cout << " out addMember" << std::endl;
}

void Channel::removeMember(Client *_client)
{
    _clientList.erase(_client);
}

bool Channel::isMember(Client *_client)
{
    return _clientList.find(_client) != _clientList.end();
}

void Channel::sendToMembers(std::string message, int fd)
{
    std::set<Client *>::iterator iterator;
    for (iterator = _clientList.begin(); iterator != _clientList.end(); iterator++)
    {
        if ((*iterator)->getFd() != fd)
        {
            if (send((*iterator)->getFd(), message.c_str(), message.length(), 0) == -1)
                perror ("send");
        }
    }
}

void Channel::sendToOne(int fd, std::string message)
{
    if (send(fd, message.c_str(), message.length(), 0) == -1)
        perror ("send");
}

Client * Channel::getMemberByNick(std::string nick)
{
    std::set<Client *>::iterator it = _clientList.begin();
    while (it != _clientList.end())
    {
        if ((*it)->getNick() == nick)
            return *it;
        it++;
    }
    return NULL;
}

bool Channel::isOperator(Client *_client)
{
    if (this->_operators.find(_client->getNick()) == this->_operators.end())
    {
        sendToOne(_client->getFd(), _client->getNick() + " " + _name + " :You're not channel operator\n" );
        return false;
    }
    return true;
}

void Channel::setModes(std::string _mode, Client *client, std::string arg)
{
    if (isOperator(client))
    {
        if (_mode == "+t" || _mode == "+i")
            this->_modes.insert(_mode);
        else if (_mode == "-t")
            this->_modes.erase("+t");
        else if (_mode == "-i")
            this->_operators.erase("+i");
        else if (_mode == "+o")
        {
            if (!getMemberByNick(arg))
                sendToOne(client->getFd(), client->getNick() + " " + _name +  " :No such nick/channel" );
            else
                _operators.insert(arg);
        }
        else if (_mode == "-o")
        {
            if (!getMemberByNick(arg))
                sendToOne(client->getFd(), client->getNick() + " " + _name +  " :No such nick/channel");
            else
                _operators.erase(arg);
        }
        else if (_mode == "+k")
            this->_password = arg;
        else if (_mode == "-k")
            this->_password = "";
        std::cout << _password << std::endl;
    }
}

bool Channel::checkModes(std::string _mode)
{
    if (this->_modes.find(_mode) != this->_modes.end())
        return true;
    return false;
}