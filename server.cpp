#include "server.hpp"
#include "sys/socket.h"

//The setsockopt() function allows the user to set various socket options, such as the socket's timeout value, the maximum size of the socket's send and receive buffer
const char *Server::ProblemInFdServer::what() const throw()
{
    return ("Problem in fd server!");
}
const char *Server::ProblemInSockOpt::what() const throw()
{
    return ("Problem in reusing socket / port!");
}
const char *Server::ProblemInPoll::what() const throw()
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
    delete this->client;
}

Server::Server()
{
    std::cout << "Constructor for server is called" << std::endl;

}

void Server::launch_socket()
{
    //This struct stores the socket address information of the server.
    sockaddr_in sockaddr;
    // initialized to zero
    std::memset(&sockaddr, 0, sizeof sockaddr);
  
    // the protocol family.
    sockaddr.sin_family = FAMILY;
    // IP address of the server.
    sockaddr.sin_addr.s_addr = ADDR;

    // the port number
    sockaddr.sin_port = htons(this->port);
    int flag = 1;

    //This creates a new socket and assigns 
    // the file descriptor 
    // AF_INET specifies the protocol family 
    //SOCK_STREAM the socket type (in this case, a TCP socket)
    //0 specifies the protocol (the default protocol for TCP).
    this->fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (this->fd < 0)
        throw Server::ProblemInFdServer();

    //this function sets the SO_REUSEADDR to 0 
    //to allow the reuse of the local adress 
    //when bining the socket
    if(setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0)
        throw Server::ProblemInSockOpt();
    
    //this function sets the fd as non-blocking , which means that recv will
    //not wait for data to be received, which means the server
    //will handle multiple clients at once.
    if (fcntl(this->fd, F_SETFL, O_NONBLOCK) < 0)
        throw Server::ProblemInFcntl();
    
    //binds the socket to the IP address and 
    //port number specified in the sockaddr struct
    if(bind(this->fd,(struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
         throw Server::ProblemInBindServer();
    
    //function is used to listen for incoming connections on a socket
    if (listen(this->fd, 32) < 0)
         throw Server::ProblemInlistenServer();
    return ;
}


void Server::receive_message(std::vector<pollfd>::iterator i, Client *client, int len)
{
    if (len < 0)
        return ;
    else
    {
        if (len == 0)
        {
            close(i->fd);
            this->poll_vec.erase(i);
            std::cout << "client went away!!" << std::endl;
            return ;
        }
        else
        {
            // std::cout << "new buffer" << std::endl;
            std::string buff = this->buffer;
            client->buff_client.append(buff);
            // std::cout << "This is the client->buff_client :" << client->buff_client;
            std::size_t pos = 0;
            while ((pos = client->buff_client.find("\r\n", pos)) != std::string::npos) 
            {
                client->buff_client.replace(pos, 2, "\n");
                // std::cout << "replacing" << std::endl;
            }
            if (client->buff_client.find('\n') != std::string::npos && client->buff_client.size() > 1)
            {
                 // puts("found new line");
                // std::cout << "new line found" << std::endl;
                if (!client->checkIsRegister())
                    client_not_connected(client);
                else
                    client_connected(client);
            } 
        }
    }
}


void Server::client_connected(Client *client)
{

    puts("enter");
   std::vector<std::string> command_split;
   std::string key;
   size_t end_it = 0;
   size_t pos_it = 0;

    pos_it = 0;
    while((end_it = client->buff_client.find(" ", pos_it)) != std::string::npos)
    {
            key  = client->buff_client.substr(pos_it, end_it - pos_it);
            command_split.push_back(key);
            pos_it = end_it + 1;
    }

    if (pos_it < client->buff_client.length())
    {
            key = client->buff_client.substr(pos_it, client->buff_client.length() - pos_it);
            command_split.push_back(key);
    }
    //display the output
    std::string m = command_split[command_split.size() - 1];
    m[m.length() - 1] = '\0';
    command_split[command_split.size() - 1] = m;
    client->setCommand(command_split);
    _execute_commands(client);
    client->commande_splited.clear();
    client->buff_client.clear();
    command_split.clear();

    // std::size_t x = 0;
    // while (x < client->commande_splited.size())
    // {
    //     std::cout << x << " : {" <<  command_split[x] << "} \n";
    //     x++;
    // }
}

void Server::client_not_connected(Client *client)
{
    // puts("dante");
   std::vector<std::string> message_split;
   std::string command;
   size_t pos = 0;
   size_t end = 0;

   std::vector<std::string> command_split;
   std::string key;
   size_t end_it = 0;
   size_t pos_it = 0;

   std::size_t k = 0;
 
   while((end = client->buff_client.find("\n", pos)) != std::string::npos)
   {
        command  = client->buff_client.substr(pos, end - pos);
        message_split.push_back(command);
        pos = end + 1;
   }

   if (pos < client->buff_client.length())
   {
        command = client->buff_client.substr(pos, client->buff_client.length() - pos);
        message_split.push_back(command);
   }
 
    // for (unsigned int i = 0;i < message_split.size();++i)
    //         std::cerr << message_split[i];
    
   while (k < message_split.size())
   {
        pos_it = 0;
        while((end_it = message_split[k].find(" ", pos_it)) != std::string::npos)
        {
                key  = message_split[k].substr(pos_it, end_it - pos_it);
                command_split.push_back(key);
                pos_it = end_it + 1;
        }

        if (pos_it < message_split[k].length())
        {
                key = message_split[k].substr(pos_it, message_split[k].length() - pos_it);
                command_split.push_back(key);
        }
        //execute
        std::cerr << "{";
        for (unsigned int i = 0;i < command_split.size();++i)
            std::cerr << command_split[i] << " ";
        std::cerr << "}\n";
        this->_command_splited = command_split;
        
        if (_isNotChannelCmd(command_split)) {
            client->setCommand(command_split);
            client->execute();
        }
        command_split.clear();
        //should i clear it ?
        this->client->buff_client.clear();
        ++k; 
   }

    //display the output 
    // std::size_t x = 0;
    // while (x < client->commande_splited.size())
    // {
    //     std::cout << x << "the splited command : {" <<  command_split[x] << "} \n";
    //     x++;
    // }
}


Server::Server(int port, std::string password): password(password), port(port) , off(FALSE)
{
    //create the server socket
    this->launch_socket();

    pollfd server_poll;
    std::memset(&server_poll, 0, sizeof(server_poll)); //initialize the structure with zeros


    struct sockaddr_in addr_client;
    socklen_t len = sizeof(addr_client);
    struct pollfd client_poll;
    int client_fd;

    server_poll.fd = this->fd;
    server_poll.events = POLLIN; //should monitor for incoming data

    this->poll_vec.push_back(server_poll); //poll_vec is a vector of pollfd where to stock infos about each connection to the server

    while(this->off == FALSE)
    {
        if(poll(poll_vec.data(), this->poll_vec.size(), 0) < 0)
            throw Server::ProblemInPoll();
         //an iterator that points on the elements of the pollfd vector
        //iterate through the poll_vec
        for (unsigned long i = 0; i < poll_vec.size(); i++)
        {
            pollfd& current = poll_vec[i];
            if (!(current.revents & POLLIN))
                continue;
            if (current.fd == this->fd) //checks if the current socket is the server socket to accept a new client
            {
               try
               {
                
                    client_fd = accept(this->fd, (struct sockaddr*)&addr_client,&len);
                    if (client_fd < 0)
                        throw std::runtime_error("Problem in accept client: ");
                    fcntl(client_fd, F_SETFL, O_NONBLOCK); //sets the client socket to non-blocking
                    client_poll.fd = client_fd;
                    client_poll.events = POLLIN; //monitor incoming data on client
                    client = new Client(client_fd, *this);
                    this->clients.insert(std::make_pair(client_fd, client));
                    poll_vec.push_back(client_poll);
                    // std::cout << "client fd: " << client_fd << " current.fd" <<current.fd << std::endl;
                    std::cout << "pushed to the vector" << std::endl;
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
                std::memset(&this->buffer, 0, sizeof(this->buffer));
                int len = recv(current.fd, this->buffer, 500, 0);
                // std::cout << "This is the this->buffer value: " << this->buffer << std::endl; 
                receive_message(i + poll_vec.begin(), clients[current.fd], len);
                size_t pos = this->client->buff_client.find("\n");
                if ( pos != std::string::npos) {
                     this->client->buff_client =  this->client->buff_client.substr(pos + 1,  this->client->buff_client.size());
                }
            }
        }
    }
}

void Server::sendMsg(int fd, std::string msg)
{
    if (send(fd, msg.c_str(), msg.length(), 0) == -1)
    {
        perror("send");
        return;
    }
}

bool Server::_isNotChannelCmd(std::vector<std::string> command_splited)
{
    if (command_splited[0] == "USER" || command_splited[0] == "user")
        return true;
    if (command_splited[0] == "PASS" || command_splited[0] == "pass")
        return true;
    if (command_splited[0] == "NICK" || command_splited[0] == "nick")
        return true;
    return false;
}

bool Server::findNick(std::string &nick)
{
    for (std::map<int, Client*>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
    {
        if (it->second->getNick() == nick)
            return true;
    }
    return false;
}

void Server::printAllClients()
{
    for (std::map<int, Client*>::iterator it = this->clients.begin(); it != this->clients.end();++it)
    {
        std::cout << it->second->getNick() << std::endl;
    }
}


void Server::_execute_commands(Client *client) 
{
    if (client->commande_splited[0] == "JOIN" || client->commande_splited[0] == "join")
    {
    std::cout << "sorti *** " << std::endl;
        _joinCmd(client);
    }
    std::cout << "sorti" << std::endl;
}

void Server::_joinCmd(Client *client)
{
    int index;
    int size = client->commande_splited.size();
    std::vector<std::string> passwords;
    std::vector<std::string> channels;
    index = 1;
    while (client->commande_splited[index][0] == '#')
        channels.push_back(client->commande_splited[index++]);
    if (index == 1)
        sendMsg(client->getFd(), "Channel name has to start by #\n");
    else
    {
        while(index < size)
            passwords.push_back(client->commande_splited[index++]);
        passwords.resize(channels.size(), "");
        for (size_t i = 0; i < passwords.size(); i++)
        {
            std::cout << passwords[i] << " pass" << std::endl;
            std::cout << channels[i] << " channel" << std::endl;
        }
        size = channels.size();
        for (int i = 0; i < size; i++)
        {
            Channel *channel = _channels.find(channels[i])->second;
            if (!channel || channel == _channels.end()->second)
            {
                Channel *newChanel = new Channel(channels[i], client->getFd(), passwords[i]);
                _channels.insert(make_pair(channels[i], newChanel));
            }
            else
                channel->addMember(client->getFd(), passwords[i]);
        }
    }
}


