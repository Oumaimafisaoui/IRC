#include "channel.hpp"

Channel::Channel(std::string name, Client *_client)
{
    this->_name = name;
    this->_owner = _client;
    this->_operators.insert(_client->getNick());
    _clientList.insert(_client);
    this->_password = "";
    this->_inviteMode = false;
    this->_topicMode = false;
    this->_topic = "";
    sendToOne(_client->getFd(), ":" + _client->get_nick_adresse(NULL) + " " + "JOIN" + " :" + _name);
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
    if (this->_inviteMode)
    {
        if (invitedLists.find(_client->getNick()) == invitedLists.end())
        {
            sendToOne(_client->getFd(), ":IRC 473 " + _client->getNick() + " " + _name +  " :" + "Cannot join channel (+i)" );
            return ;
        }
    }
    if (password != this->_password && this->_password != "")
        sendToOne(_client->getFd(), ":IRC 475 " +_client->getNick() + " " + _name +  " :" + "Cannot join channel (+k)" );
    else if (isMember(_client))
        sendToOne(_client->getFd(), ":IRC 443" +_client->getNick() + " " + _name +  " :" + "You've already joined this channel");
    else
    {
        _clientList.insert(_client);
        sendToMembers(":" + _client->get_nick_adresse(NULL) + " JOIN" + " :" + _name);
        if (_topic != "")
            sendToOne(_client->getFd(), ":" + _client->get_nick_adresse(NULL) + " TOPIC" + " :" + _topic);
    }
}

bool Channel::isMember(Client *_client)
{
    return _clientList.find(_client) != _clientList.end();
}

void Channel::sendToMembers(std::string message)
{
    std::set<Client *>::iterator iterator;
    message += "\r\n";
    for (iterator = _clientList.begin(); iterator != _clientList.end(); iterator++)
    {
        if (send((*iterator)->getFd(), message.c_str(), message.length(), 0) == -1)
            perror ("send");
    }
}

void Channel::sendToOne(int fd, std::string message)
{
    message += "\r\n";
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
        sendToOne(_client->getFd(),":IRC 475 " +_client->getNick() + " " + _name +  " :" +  " :You're not channel operator" );
        return false;
    }
    return true;
}

void Channel::setModes(std::string _mode, Client *_client, std::string arg)
{
    if (isOperator(_client))
    {
        if (_mode == "+t")
        {
            sendToMembers(":" + _client->get_nick_adresse(NULL) + " " + "MODE " + _name + " +t");
            this->_topicMode = true;
        }
        else if (_mode == "+i")
        {
            sendToMembers(":" + _client->get_nick_adresse(NULL) + " " + "MODE " + _name + " +i");
            this->_inviteMode = true;
        }
        else if (_mode == "-t")
        {
            sendToMembers(":" + _client->get_nick_adresse(NULL) + " " + "MODE " + _name + " -t");
            this->_topicMode = false;
        }
        else if (_mode == "-i")
        {
            sendToMembers(":" + _client->get_nick_adresse(NULL) + " " + "MODE " + _name + " -i");
            this->_inviteMode = false;
        }
        else if (_mode == "+o")
        {
            if (!getMemberByNick(arg))
                sendToOne(_client->getFd(),  ":IRC 441 " + _client->getNick() + " " + _name +  " :" + "They aren't on that channel" );
            else
            {
                sendToMembers(":" + _client->get_nick_adresse(NULL) + " " + "MODE " + _name + " +o " + arg);
                _operators.insert(arg);
            }
        }
        else if (_mode == "-o")
        {
            if (!getMemberByNick(arg))
                sendToOne(_client->getFd(),  ":IRC 441 " + _client->getNick() + " " + _name +  " :" + "They aren't on that channel" );
            else
            {
                sendToMembers(":" + _client->get_nick_adresse(NULL) + " " + "MODE " + _name + " -o " + arg);
                _operators.erase(arg);
            }
        }
        else if (_mode == "+k")
        {
            sendToMembers(":" + _client->get_nick_adresse(NULL) + " " + "MODE " + _name + " +k " + arg);
            this->_password = arg;
        }
        else if (_mode == "-k")
        {
            sendToMembers(":" + _client->get_nick_adresse(NULL) + " " + "MODE " + _name + " -k " + arg);
            this->_password = "";
        }
    }
}

void Channel::setTopic(std::string newTopic, Client *_client, int n)
{
    if (!isMember(_client))
    {
        sendToOne(_client->getFd(), ":IRC 442 " + _client->getNick() + " " + _name +  " :You're not on that channel");
        return ;
    }
    if (_topicMode)
    {
        if (!this->isOperator(_client))
            return ;
    }
    if (n)
    {
        if (_topic == "")
            sendToOne(_client->getFd(), ":IRC 331 " + _client->getNick() + " " + _name +  " :No topic is set.");
        else
            sendToOne(_client->getFd(), ":" + _client->get_nick_adresse(NULL) + " " + "TOPIC "  + _name + " " + _topic);
    }
    else
    {
        _topic = newTopic;
        sendToMembers(":" + _client->get_nick_adresse(NULL) + " " + "TOPIC "  + _name + " " + _topic);
    }
}

void Channel::addInvited(std::string nick, Client *_client, Client *invited)
{
    if (!isMember(_client))
    {
        sendToOne(_client->getFd(), ":IRC 442 " + _client->getNick() + " " + _name +  " :You're not on that channel");
        return ;
    }
    if (!this->isOperator(_client))
        return ;
    if (getMemberByNick(nick))
    {
        sendToOne(_client->getFd(), ":IRC 443 " + _client->getNick() +  " " + nick + " " + _name +  " :is already on channel\n" );
        return ;
    }
    invitedLists.insert(nick);
    sendToOne(_client->getFd(), ":IRC 341 " + _client->getNick() + " " + nick + " " + _name);
    sendToOne(_client->getFd(), "invited " + nick + " into channel " + _name);
    sendToOne(invited->getFd(), ":" + _client->get_nick_adresse(NULL) + " INVITE " + nick + " :" + _name);
    std::cout << "Channel name " << _name << std::endl; 
}

void Channel::removeMember(Client *_client, std::string raison)
{
    (void)raison;
    if (!isMember(_client))
    {
       sendToOne(_client->getFd(), ":IRC 442 " + _client->getNick() + " " + _name +  " :You're not on that channel");
        return ;
    }
    sendToMembers(":" + _client->get_nick_adresse(NULL) + " " + "PART " + _name + " " + raison);
    clearMember(_client);
}

void Channel::kickClient(Client *_client, std::string nick, std::string comment) 
{
    if (!isMember(_client))
    {
        sendToOne(_client->getFd(), ":IRC 442 " + _client->getNick() + " " + _name +  " :You're not on that channel");
        return ;
    }
    if (!isOperator(_client))
        return ;
    Client *user = getMemberByNick(nick);
    if (!user)
    {
        sendToOne(_client->getFd(), ":IRC 401 " + _client->getNick() +  " " + nick + " :No such nick/channel\r\n");
        return ;
    }
    if (comment == "")
        comment = "The raison has not mentioned";
    sendToMembers(":" + _client->get_nick_adresse(NULL) + " KICK " + _name  + " " + nick + " " + comment);
    clearMember(user);
}

bool Channel::isOperator_wm(Client *_client)
{
    if (this->_operators.find(_client->getNick()) == this->_operators.end())
        return false;
    return true;
}

void Channel::clearMember(Client *_client)
{
    _clientList.erase(_client);
    if (this->_operators.find(_client->getNick()) != this->_operators.end())
        _operators.erase(_client->getNick());
    if (this->invitedLists.find(_client->getNick()) != this->invitedLists.end())
        invitedLists.erase(_client->getNick());
}

void Channel::removeIt(Client *_client)
{
    if (isMember(_client))
    {
        _clientList.erase(_client);
        return ;
    }
    return ;
}