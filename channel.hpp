#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <set>
#include <string>
#include <sys/socket.h>
#include <sys/socket.h>
#include <utility>
#include "client.hpp"


class Channel {
    private:
        std::set<Client *> _clientList;
        std::string _name;
        std::set<std::string> _operators;
        bool _inviteMode;
        bool _topicMode;
        std::set<std::string> invitedLists;
        std::string _password;
        Client *_owner;
        std::string _topic;
    public:
        Channel(std::string name, Client *_client, std::string password);
        std::string getChannelName();
        std::set<Client *> getClients();
        void addMember(Client *_client, std::string password);
        void removeMember(Client *_client, std::string raison);
        bool isMember(Client *_client);
        void sendToMembers(std::string message, int fd);
        void sendToOne(int fd, std::string message);

        void setTopic(std::string _topic, Client *_client, int n);
        void setModes(std::string _mode, Client *client, std::string arg);
        void addOperator(std::string nick);
        void removeOperator(std::string nick);
        bool isOperator(Client *client);
        void addInvited(std::string nick, Client *_client);
        void removeInvited(std::string nick);
        void isInvited(std::string nick);
        void setPassword(std::string _password);
        Client *getMemberByNick(std::string nick);
        bool checkModes(std::string _mode);
};

#endif