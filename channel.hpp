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
        std::set<std::string> operators;
        std::set<std::string> invitedLists;
        std::string _password;
        Client *_owner;
        std::string _topic;
    public:
        Channel(std::string name, Client *_client, std::string pass);
        std::string getChannelName();
        std::set<Client *> getClients();
        void addMember(Client *_client, std::string password);
        void removeMember(Client *_client);
        bool isMember(Client *_client);
        void sendToMembers(std::string message);
        void sendToOne(int fd, std::string message);

        void setTopic(std::string _topic);
        void setModes(std::string _mode);
        void addOperator(std::string nick);
        void removeOperator(std::string nick);
        bool isOperator(std::string nick);
        void addInvited(std::string nick);
        void removeInvited(std::string nick);
        void isInvited(std::string nick);
        void setPassword(std::string nick);
};

#endif