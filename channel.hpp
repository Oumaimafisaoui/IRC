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
        std::set<int> _clientList;
        std::string _name;
        std::string _password;
        int _ownerFd;
    public:
        Channel(std::string name, int ownerFd, std::string pass);
        std::string getChannelName();
        std::set<int> getClients();
        void addMember(int fd, std::string password);
        void removeMember(int fd);
        bool isMember(int fd);
        void sendToMembers(std::string message);
        void sendToOne(int fd, std::string message);
};

#endif