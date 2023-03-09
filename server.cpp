#include "server.hpp"

//The setsockopt() function allows the user to set various socket options, such as the socket's timeout value, the maximum size of the socket's send and receive buffer
const char *Server::ProblemInFdServer::what() const throw()
{
    return ("Problem in fd server!");
}
const char *Server::ProblemInSockOpt::what() const throw()
{
    return ("Problem in reusing socket / port!");
}
const char *Server::ProblemInFcntl::what() const throw()
{
    return ("Problem in non blocking mode!");
}
const char *Server::ProblemInBindServer::what() const throw()
{
    return ("Problem in binding server!");
}
const char *Server::ProblemInlistenServer::what() const throw()
{
    return ("Problem in listening server!");
}
const char *Server::ProblemInConnectServer::what() const throw()
{
    return ("Problem in connection server!");
}

std::string Server::get_pass() const
{
    return (this->password);
}
int Server::get_port() const
{
    return (this->port);
}
int Server::get_fd() const
{
    return (this->fd);
}


Server::~Server(void)
{
    std::cout << "Distructor for server is called" << std::endl;
}

Server::Server()
{
    std::cout << "Constructor for server is called" << std::endl;

}

void Server::launch_socket()
{
    sockaddr_in sockaddr;
    std::memset(&sockaddr, 0, sizeof sockaddr);
  
    sockaddr.sin_family = FAMILY;
    sockaddr.sin_addr.s_addr = ADDR;
    sockaddr.sin_port = htons(this->port);
    int flag = 1;


    this->fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (this->fd < 0)
        throw Server::ProblemInFdServer();
    if(setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0)
        throw Server::ProblemInSockOpt();
    fcntl(this->fd, F_SETFL, O_NONBLOCK);
    if(bind(this->fd,(struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
         throw Server::ProblemInBindServer();
    if (listen(this->fd, 32) < 0)
         throw Server::ProblemInlistenServer();
    return ;
}


void Server::receive_message(int fd, std::vector<pollfd>::iterator iter)
{
    int len;
    std::string message = "";

    // Client *client = new Client(fd);
    // fcntl(fd, F_SETFL, O_NONBLOCK);
    (void) iter;
    len = recv(fd, this->buffer, 500, 0);
    buffer[len] = 0;
    std::cout << buffer << std::endl;
    if (len < 0){
    }
    else
    {
        if (len == 0)
        {
            std::cout << "client went away!!" << std::endl;
            close(fd);
            poll_vec.erase(iter);
            return ;
        }
        else
        {
            this->buffer[len] = 0;
            message.append(buffer);
        }
    }
    std::cout << "received :" << buffer<< std::endl; 
    std::size_t j = message.find("\n\r");
    while(j != message.npos)
    {
        message.erase(j, 1);
        message.insert(j, " ");
    }
}

Server::Server(int port, std::string password): fd(0), password(password), port(port) , off(FALSE)
{
    //create poll
    this->launch_socket();
    pollfd server_poll;
    std::memset(&server_poll, 0, sizeof(server_poll));

    struct sockaddr_in addr_client;
    socklen_t len = sizeof(addr_client);
    struct pollfd client_poll;
    int client_fd;

    //this configuration for the server socket, we want to poll for incoming data on the server socket
    server_poll.fd = this->fd;
    server_poll.events = POLLIN;

    this->poll_vec.push_back(server_poll);

    while(this->off == FALSE)
    {
        if(poll(poll_vec.data(), this->poll_vec.size(), 0) < 0)
            perror("poll: failed");
        std::vector<pollfd>::iterator i;
        for (i = this->poll_vec.begin(); i < poll_vec.end(); i++)
        {
            if (! (i->revents & POLLIN))
                continue;
                if (i->fd == this->fd)
                {
                    std::cout << "Hell no\n";
                   try
                   {
                        client_fd = accept(this->fd, (struct sockaddr*)&addr_client,&len);
                        std::cout << client_fd << std::endl;
                        if (client_fd < 0)
                        {
                            throw std::runtime_error("Problem in accept client: " + std::string(strerror(errno)));
                        }
                        std::cout << "hello there again" << std::endl;
                        fcntl(client_fd, F_SETFL, O_NONBLOCK);
                        client_poll.fd = client_fd;
                        client_poll.events = POLLIN;
                        client_poll.revents = 0;
                        i->revents = 0;
                        poll_vec.push_back(client_poll);
                        std::cout << "pushed" << std::endl;
                   }
                   catch(const std::exception& e)
                   {
                        std::cerr << e.what() << '\n';
                        close(client_fd);
                   }
                   continue;
                }
                else 
                {
                    receive_message(i->fd, i);    
                }
        }
    }
}





