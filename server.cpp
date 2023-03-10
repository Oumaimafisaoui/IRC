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
    this->message = "";
    
    buffer[len] = 0;
    if (len < 0)
        return ;
    else
    {
        if (len == 0)
        {
            std::cout << "client went away!!" << std::endl;
            close(i->fd);
            this->poll_vec.erase(i);
            return ;
        }
        else
        {
            this->buffer[len] = 0;
            message.append(buffer);
            std::size_t j = message.find("\n\r");
            while(j != message.npos)
            {
                //remove "\r"
                message.erase(j, 1);
                message.insert(j,"");
            }
        }
    }
 
// client_not_connected(message ,client);
client_connected(message, client);
}


void Server::client_connected(std::string message , Client *client)
{

    (void)client;

   std::vector<std::string> command_split;
   std::string key;
   size_t end_it = 0;
   size_t pos_it = 0;

    pos_it = 0;
    while((end_it = message.find(" ", pos_it)) != std::string::npos)
    {
            key  = message.substr(pos_it, end_it - pos_it);
            command_split.push_back(key);
            pos_it = end_it + 1;
    }

    if (pos_it < message.length())
    {
            key = message.substr(pos_it, message.length() - pos_it);
            command_split.push_back(key);
    }

    //display the output 
    client->setCommand(command_split);
    std::size_t x = 0;
    while (x < client->commande_splited.size())
    {
        std::cout << x << " : {" <<  command_split[x] << "} \n";
        x++;
    }
}

void Server::client_not_connected(std::string message , Client *client)
{
   std::vector<std::string> message_split;
   std::string command;
   size_t pos = 0;
   size_t end = 0;

   std::vector<std::string> command_split;
   std::string key;
   size_t end_it = 0;
   size_t pos_it = 0;

   std::size_t k = 0;

   while((end = message.find("\n", pos)) != std::string::npos)
   {
        command  = message.substr(pos, end - pos);
        message_split.push_back(command);
        std::cout << command << std::endl;
        pos = end + 1;
   }

   if (pos < message.length())
   {
        command = message.substr(pos, message.length() - pos);
        message_split.push_back(command);
   }

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
        k++; 
   }

    //display the output 
    client->setCommand(command_split);
    std::size_t x = 0;
    while (x < client->commande_splited.size())
    {
        std::cout << x << " : {" <<  command_split[x] << "} \n";
        x++;
    }
}


Server::Server(int port, std::string password): password(password), port(port) , off(FALSE)
{
    //create the server socket
    this->launch_socket();

    //Create the pollfd structure
    //pollfd is a structure represent a file descriptor to be monitored by the poll() system call.
    pollfd server_poll;
    std::memset(&server_poll, 0, sizeof(server_poll)); //initialize the structure with zeros


    //client adrr infos
    struct sockaddr_in addr_client;
    socklen_t len = sizeof(addr_client);
    struct pollfd client_poll;
    int client_fd;

    //this configuration for the server socket, we want to poll for incoming data on the server socket
    server_poll.fd = this->fd;
    server_poll.events = POLLIN; //should monitor for incoming data

    this->poll_vec.push_back(server_poll); //poll_vec is a vector of pollfd where to stock infos about each connection to the server

    while(this->off == FALSE)
    {
        //this is the poll function that is used to monitor each connection
        //poll_vec.data() => a pointer to the first element of poll_vec, list of fds to be monitored
        //this->poll_vec.size() => the number of elements in poll_vec, rn is 1
        // the amount of time to wait for an event to accure
        if(poll(poll_vec.data(), this->poll_vec.size(), 0) < 0)
            throw Server::ProblemInPoll();

         //an iterator that points on the elements of the pollfd vector
        //iterate through the poll_vec
        for (unsigned long i = 0; i < sizeof(poll_vec); i++)
        {
            //skip the current itteration if there is no incoming data on the current socket
            pollfd& current = poll_vec[i];
            if (!(current.revents & POLLIN))
                continue;
                if (current.fd == this->fd) //checks if the current socket is the server socket to accept a new client
                {
                   try
                   {
                         //accept the incoming client connection
                        client_fd = accept(this->fd, (struct sockaddr*)&addr_client,&len);
                        if (client_fd < 0)
                            throw std::runtime_error("Problem in accept client: ");
                        fcntl(client_fd, F_SETFL, O_NONBLOCK); //sets the client socket to non-blocking
                        //fill the client's pollfd struct with infos on the client
                        client_poll.fd = client_fd;
                        client_poll.events = POLLIN; //monitor incoming data on client
                        //In the next iteration, the poll function will update
                        //the revents field to reflect the new events that have occurred since the last call to poll
                        //If we don't clear the revents field, the previous events
                        // will still be present and may cause incorrect behavior in our program.
                    
                        client = new Client(fd);
                        //adds the client_poll structure to the poll_vec vector
                        poll_vec.push_back(client_poll);
                        std::cout << "pushed to the vector" << std::endl;
                        client->setFd(fd);
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
                    //If the event was on the server socket, 
                    //a new client connection is accepted using the accept() function,
                    //and the client socket is added to the poll_vec vector. 
                    //If the event was on a client socket, 
                    //the server calls the receive_message() function 
                    //to handle the incoming data.
                    int len = recv(poll_vec[i].fd, this->buffer, 500, 0);
                    receive_message(i + poll_vec.begin(), client, len);    
                }
        }
    }
}





