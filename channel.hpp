#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <map>
#include <string>
#include <sys/socket.h>
#include <sys/socket.h>
#include <utility>

class Channel {
    private:
        std::map<int, std::string> _clientList;
        std::string _name;
        int _ownerFd;
    public:
        Channel(std::string name, int ownerFd);
        std::string getChannelName();
        std::map<int, std::string> getClients();
        void addMember(int fd, std::string nickName);
        void removeMember(int fd);
        bool isMember(int fd);
        void sendToMembers(std::string message);
};

#endif