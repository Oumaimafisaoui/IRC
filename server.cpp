#include "server.hpp"

Server::~Server(void)
{
    std::cout << "Distructor for server is called" << std::endl;
}

Server::Server()
{
    std::cout << "Constructor for server is called" << std::endl;

}
Server::Server(int port, std::string password)
{
    sockaddr_in sockaddr;

    std::memset(&sockaddr, 0, sizeof sockaddr);
    sockaddr.sin_family = FAMILY;
    sockaddr.sin_addr.s_addr = ADDR;
    sockaddr.sin_port = htons(port);

    this->port = port;
    this->password = password;
    this->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->fd == -1)
    {
        std::cout << "Problem in socket creation" << std::endl;
        exit(1);
    }
    if(bind(this->fd,(struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
    {
        std::cout << "Problem in binding socket" << std::endl;
        exit(1);
    }
    if (listen(this->fd, 1) < 0)
    {
        std::cout << "Problem in listening" << std::endl;
        exit(1);
    }

}