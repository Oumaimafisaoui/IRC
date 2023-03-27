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

<<<<<<< HEAD
=======
size_t handle_response(char* data, size_t size, size_t nmemb, std::string* buffer) {
    size_t content_start = 0, content_end = 0;

    if (buffer != nullptr) {
        std::string str(data, size * nmemb);
        content_start = str.find("\"content\":\"") + 11;
        content_end = str.find("\",\"author\":");

        if (content_start != std::string::npos && content_end != std::string::npos) {
            buffer->clear(); // clear the buffer first
            buffer->append(str.substr(content_start, content_end - content_start));
        }
    }
    return size * nmemb;
}

void Server::_botCmd(Client *client)
{
    (void)client;
    // Initialize the global curl library
    curl_global_init(CURL_GLOBAL_DEFAULT);
    // Initialize the easy curl handle
    CURL *curl = curl_easy_init();

    // Check for errors
    if (!curl) {
        return ;
    }

    // Set the API URL
    std::string api_url = "https://api.quotable.io/random";
    curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());

    // Set the callback function for handling the response
    std::string buffer;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    // Execute the API call
    CURLcode res = curl_easy_perform(curl);

    // Check for errors
    if (res != CURLE_OK) {
        return ;
    }

    // Cleanup libcurl
    curl_easy_cleanup(curl);
    // Cleanup the global curl library
    curl_global_cleanup();

    // Parse the JSON response
    size_t content_start = buffer.find("\"content\":\"") + 11;
    size_t content_end = buffer.find("\",\"author\":");
    std::string quote = buffer.substr(content_start, content_end - content_start);

    sendMsg(client->getFd(), quote);
}
>>>>>>> a3750cbfa676242cdb642372bce635ac95cd626f

void Server::_botCmd(Client *client)
{
    if(client->commande_splited.size() < 2)
    {
        sendMsg(client->getFd(), " BOT :Welcome to IRC Bot ! Please insert /bot Quote or /bot Joke or /bot Time and enjoy!\r\n");
        return ;
    }
    if (client->commande_splited.size() > 3)
    {
        sendMsg(client->getFd(), " BOT :Too much arguments!\r\n");
        return ;
    }
    else if (client->commande_splited[1] == "Quote")
    {
        std::ifstream file("quotes.txt");
        std::string quote;
        int line = rand() % 14;
        for (int i = 0; i < line; i++)
            std::getline(file, quote);
        file.close();
        sendMsg(client->getFd(), "Hello "+ client->getNick() + " ,Here is your Quote for today: " + quote + "\r\n");
    }
    else if (client->commande_splited[1] == "Joke")
    {
        std::ifstream file("jokes.txt");
        std::string quote;
        int line = rand() % 10;
        for (int i = 0; i < line; i++)
            std::getline(file, quote);
        file.close();
        sendMsg(client->getFd(), "Hello "+ client->getNick() + " ,Here is your Joke for today: " + quote + "\r\n");
    }
    else if (client->commande_splited[1] == "Time")
    {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        std::ostringstream oss;
        oss << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec;
        std::string time = oss.str();
        sendMsg(client->getFd(), "Hello "+ client->getNick() + " ,Here is the time: " + time + "\r\n");

    }
    else
    {
        sendMsg(client->getFd(), "461 " + client->getNick() + " BOT :Wrong command!\r\n");
        return ;
    }
    
}



Server::~Server(void)
{
    std::cout << "Distructor for server is called" << std::endl;
    delete this->client;
}

Server::Server()
{
    is_active = false;
    std::cout << "Constructor for server is called" << std::endl;

}

Client *Server::find_client(const std::string &nick) const
{
    std::map<int, Client*>::const_iterator i;
    for(i = clients.cbegin(); i != clients.cend(); i++)
    {
        if (i->second->getNick() == nick)
                return i->second;
    }
    return (NULL);
}

Channel *Server::find_channel(const std::string &name) const
{
    std::vector<Channel*>::const_iterator i;
    for(i = _channels.cbegin(); i != _channels.cend(); i++)
    {
        if ((*i)->getChannelName() == name)
                return (*i);
    }
    return (NULL);
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
            clients.erase(i->fd);
            
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
    m = m.substr(0, m.size() - 1);
    command_split[command_split.size() - 1] = m;
    client->setCommand(command_split);
    _execute_commands(client);
    client->commande_splited.clear();
    client->buff_client.clear();
    command_split.clear();
}

void Server::client_not_connected(Client *client)
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
        this->client->buff_client.clear();
        ++k; 
   }

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
                    // fcntl(client_fd, F_SETFL, O_NONBLOCK); //sets the client socket to non-blocking
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
    if (clients.empty())
        return false;
    for (std::map<int, Client*>::const_iterator it = this->clients.cbegin(); it != this->clients.cend(); ++it)
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

Channel *Server::_findChannel(std::string name)
{
    size_t size;

    size = _channels.size();
    for (size_t i = 0; i < size; i++)
    {
        if (_channels[i]->getChannelName() == name)
            return _channels[i];
    }
    return (NULL);
}

/** Needs modification : clients is not a vector object it is a map*/

Client *Server::findClientByNick(std::string nick)
{
    std::map<int, Client*>::iterator iter;
    iter = clients.begin();

    while (iter != clients.end())
    {
        if ((*iter).second->getNick() == nick)
            return  (*iter).second;
        iter++;
    }
    return (NULL);
}

std::vector<std::string> Server::joinCmdParser(std::string params)
{
    std::vector<std::string> ret;
    std::cout << "Enter in Join parser Cmd" << std::endl;
    std::string temp = "";
    for (size_t i = 0; i < params.length(); i++)
    {
        if (params[i] == ',')
        {
            ret.push_back(temp);
            temp = "";
        }
        else
            temp += params[i];
    }
    ret.push_back(temp);
    return ret;
    std::cout << "out in Join parser Cmd" << std::endl;
}


void Server::_execute_commands(Client *client) 
{
    std::cout << "enter" << std::endl;
    std::cout << client->commande_splited[0] << std::endl;
    if (client->commande_splited[0] == "JOIN" || client->commande_splited[0] == "join")
        _joinCmd(client);
    if (client->commande_splited[0] == "MODE" || client->commande_splited[0] == "mode")
        _modeCmd(client);
    if (client->commande_splited[0] == "PRIVMSG" || client->commande_splited[0] == "privmsg")
        _privMsgCmd(client);
    if (client->commande_splited[0] == "NOTICE" || client->commande_splited[0] == "notice")
        _NoticeCmd(client);
    if (client->commande_splited[0] == "TOPIC" || client->commande_splited[0] == "topic")
        _topicCmd(client);
    if (client->commande_splited[0] == "INVITE" || client->commande_splited[0] == "invite")
        _inviteCmd(client);
    if (client->commande_splited[0] == "/BOT" || client->commande_splited[0] == "/bot")
        _botCmd(client);
    if (client->commande_splited[0] == "PART" || client->commande_splited[0] == "part")
        _partCmd(client);
}


void Server::_privMsgCmd(Client *client)
{
    if (client->commande_splited.size() < 2)
    {
        sendMsg(client->getFd(), ":" + client->getHost()+ " 411 " + (client->getNick().empty() ? "*" : client->getNick()) + " " + " :No recipient given (PRIVMSG)\r\n");
        return;
    }
    // to accumalate all the arguments
    std::ostringstream oss;
    std::copy(client->commande_splited.begin(), client->commande_splited.end(), std::ostream_iterator<std::string>(oss, " "));
    std::string commands = oss.str();

    size_t dots = commands.find_last_of(":");
    std::string message;
    std::vector<std::string> targets;
    std::string command;
    size_t pos = 8;
    size_t end = 0;
    
    if (dots == std::string::npos)
    {
        sendMsg(client->getFd(), ":" + client->getHost()+ "  412  " + (client->getNick().empty() ? "*" : client->getNick()) + " " + ":No text to send\r\n");
        return;
    }
    message = commands.substr(dots, commands.size() - dots); 
    // if only 3 arguments
    if (client->commande_splited.size() == 3)
    {
        if (client->commande_splited[1].find(',') != std::string::npos)
        {
            while((end = commands.find(",", pos)) != std::string::npos)
            {
                    command  = commands.substr(pos, end - pos);
                    targets.push_back(command);
                    pos = end + 1;
            }
            if(pos < commands.length())
            {
                command = commands.substr(pos, dots - pos -1);
                targets.push_back(command);
            }
            for (size_t i = 0; i < targets.size(); i++)
            {
                std::string& target = targets[i];
                size_t pos = 0;
                while ((pos = target.find(' ', pos)) != std::string::npos)
                {
                    target.erase(pos, 1);
                }
                targets[i] = target;
                if (targets[i][0] == '#')
                {
                    Channel *tmp = find_channel(client->commande_splited[1]);
                    if (!tmp)
                    {
                        sendMsg(client->getFd(), ":" + client->getHost()+ " 403 " + (client->getNick().empty() ? "*" : client->getNick()) + " " + client->commande_splited[1] + " :No such channel\r\n");
                        return ;
                    }
                    //send message to all the clients in the channel
                    const std::set<Client*>& clients = tmp->getClients(); // create a reference to a copy of the set

                    for (std::set<Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
                    {
                        sendMsg((*it)->getFd(), ":" + client->getHost() + " G " + tmp->getChannelName() + " " + message + "\r\n");
                    }
                }
                else
                {
                    Client *tmp = find_client(targets[i]);
                    std::cout << ":" << targets[i] << ":" << std::endl;
                    if (!tmp)
                    {
                        sendMsg(client->getFd(), ":" + client->getHost()+ " 401 " + (client->getNick().empty() ? "*" : client->getNick()) + " " + " :No such nick/Channel 1\r\n");
                        return ;
                    }
                    sendMsg(tmp->getFd(), ":" + client->getHost() + " PRIVMSG " + tmp->getNick() + " " + message + "\r\n");
                }
            }
        }
        else
        {
            if (client->commande_splited[1][0] == '#')
            {
                Channel *tmp = find_channel(client->commande_splited[1]);
                if (!tmp)
                {
                    sendMsg(client->getFd(), ":" + client->getHost()+ " 403 " + (client->getNick().empty() ? "*" : client->getNick()) + " " + client->commande_splited[1] + " :No such channel\r\n");
                    return ;
                }
                //send message to all the clients in the channel
                const std::set<Client*>& clients = tmp->getClients(); // create a reference to a copy of the set

                for (std::set<Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
                {
                    sendMsg((*it)->getFd(), ":" + client->getHost() + " PRIVMSG " + tmp->getChannelName() + " " + message + "\r\n");
                }
            }
            else
            {
                Client *tmp = find_client(client->commande_splited[1]);
                if (!tmp)
                {
                    sendMsg(client->getFd(), ":" + client->getHost()+ " 401 " + (client->getNick().empty() ? "*" : client->getNick()) + " " + " :No such nick/Channel 2\r\n");
                    return ;
                }
                sendMsg(tmp->getFd(), ":" + client->getHost() + " PRIVMSG " + tmp->getNick() + " " + message + "\r\n");
            }
        }
    }
    else if (client->commande_splited.size() > 3)
    {
        while((end = commands.find(",", pos)) != std::string::npos)
        {
                command  = commands.substr(pos, end - pos);
                targets.push_back(command);
                pos = end + 1;
        }
        if(pos < commands.length())
        {
            command = commands.substr(pos, dots - pos -1);
            targets.push_back(command);
        }
        for (size_t i = 0; i < targets.size(); i++)
        {
            std::string& target = targets[i];
            size_t pos = 0;
            while ((pos = target.find(' ', pos)) != std::string::npos)
            {
                target.erase(pos, 1);
            }
            targets[i] = target;
            if (targets[i][0] == '#')
            {
                Channel *tmp = find_channel(targets[i]);
                if (!tmp)
                {
                    sendMsg(client->getFd(), ":" + client->getHost()+ " 403 " + (client->getNick().empty() ? "*" : client->getNick()) + " " + client->commande_splited[1] + " :No such channel\r\n");
                    return ;
                }
                //send message to all the clients in the channel
                const std::set<Client*>& clients = tmp->getClients(); // create a reference to a copy of the set

                for (std::set<Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
                {
                    sendMsg((*it)->getFd(), ":" + client->getHost() + " PRIVMSG " + tmp->getChannelName() + " " + message + "\r\n");
                }
            }
            else
            {
                Client *tmp = find_client(targets[i]);
                std::cout << ":" << targets[i] << ":" << std::endl;
                if (!tmp)
                {
                    sendMsg(client->getFd(), ":" + client->getHost()+ " 401 " + (client->getNick().empty() ? "*" : client->getNick()) + " " + " :No such nick/Channel 3\r\n");
                    return ;
                }
                sendMsg(tmp->getFd(), ":" + client->getHost() + " PRIVMSG " + tmp->getNick() + " " + message + "\r\n");
            }
        }
        
    }
}




void Server::_NoticeCmd(Client *client)
{
    if (client->commande_splited.size() < 2)
        return;
    // to accumalate all the arguments
    std::ostringstream oss;
    std::copy(client->commande_splited.begin(), client->commande_splited.end(), std::ostream_iterator<std::string>(oss, " "));
    std::string commands = oss.str();

    size_t dots = commands.find_last_of(":");
    std::string message;
    std::vector<std::string> targets;
    std::string command;
    size_t pos = 8;
    size_t end = 0;
    
    if (dots == std::string::npos)
        return;
    message = commands.substr(dots, commands.size() - dots); 
    if (client->commande_splited.size() == 3)
    {
        if (client->commande_splited[1].find(',') != std::string::npos)
        {
            while((end = commands.find(",", pos)) != std::string::npos)
            {
                    command  = commands.substr(pos, end - pos);
                    targets.push_back(command);
                    pos = end + 1;
            }
            if(pos < commands.length())
            {
                command = commands.substr(pos, dots - pos -1);
                targets.push_back(command);
            }
            for (size_t i = 0; i < targets.size(); i++)
            {
                std::string& target = targets[i];
                size_t pos = 0;
                while ((pos = target.find(' ', pos)) != std::string::npos)
                {
                    target.erase(pos, 1);
                }
                targets[i] = target;
                if (targets[i][0] == '#')
                {
                    Channel *tmp = find_channel(client->commande_splited[1]);
                    if (!tmp)
                        return ;
                    //send message to all the clients in the channel
                    const std::set<Client*>& clients = tmp->getClients(); // create a reference to a copy of the set

                    for (std::set<Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
                    {
                        sendMsg((*it)->getFd(), ":" + client->getHost() + " NOTICE " + tmp->getChannelName() + " " + message + "\r\n");
                    }
                }
                else
                {
                    Client *tmp = find_client(targets[i]);
                    std::cout << ":" << targets[i] << ":" << std::endl;
                    if (!tmp)
                    {
                        return ;
                    }
                    sendMsg(tmp->getFd(), ":" + client->getHost() + " NOTICE " + tmp->getNick() + " " + message + "\r\n");
                }
            }
        }
        else
        {
            if (client->commande_splited[1][0] == '#')
            {
                Channel *tmp = find_channel(client->commande_splited[1]);
                if (!tmp)
                {
                    return ;
                }
                //send message to all the clients in the channel
                const std::set<Client*>& clients = tmp->getClients(); // create a reference to a copy of the set

                for (std::set<Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
                {
                    sendMsg((*it)->getFd(), ":" + client->getHost() + " NOTICE " + tmp->getChannelName() + " " + message + "\r\n");
                }
            }
            else
            {
                Client *tmp = find_client(client->commande_splited[1]);
                if (!tmp)
                {
                    return ;
                }
                sendMsg(tmp->getFd(), ":" + client->getHost() + " NOTICE " + tmp->getNick() + " " + message + "\r\n");
            }
        }
    }
    else
    {
        while((end = commands.find(",", pos)) != std::string::npos)
        {
                command  = commands.substr(pos, end - pos);
                targets.push_back(command);
                pos = end + 1;
        }
        if(pos < commands.length())
        {
            command = commands.substr(pos, dots - pos -1);
            targets.push_back(command);
        }
        for (size_t i = 0; i < targets.size(); i++)
        {
            std::string& target = targets[i];
            size_t pos = 0;
            while ((pos = target.find(' ', pos)) != std::string::npos)
            {
                target.erase(pos, 1);
            }
            targets[i] = target;
            if (targets[i][0] == '#')
            {
                Channel *tmp = find_channel(targets[i]);
                if (!tmp)
                {
                    return ;
                }
                //send message to all the clients in the channel
                const std::set<Client*>& clients = tmp->getClients(); // create a reference to a copy of the set

                for (std::set<Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
                {
                    sendMsg((*it)->getFd(), ":" + client->getHost() + " NOTICE " + tmp->getChannelName() + " " + message + "\r\n");
                }
            }
            else
            {
                Client *tmp = find_client(targets[i]);
                std::cout << ":" << targets[i] << ":" << std::endl;
                if (!tmp)
                {
                    return ;
                }
                sendMsg(tmp->getFd(), ":" + client->getHost() + " NOTICE " + tmp->getNick() + " " + message + "\r\n");
            }
        }
        
    }
}

void Server::_joinCmd(Client *client)
{
    std::cout << "Enter in Join Cmd" << std::endl;
    int size = client->commande_splited.size();
    std::vector<std::string> passwords;
    std::vector<std::string> channels;
    Channel *newChanel;
    if (size < 2)
    {
        return ;
    }
    channels = this->joinCmdParser(client->commande_splited[1]);
    if (size > 2)
        passwords = this->joinCmdParser(client->commande_splited[2]);
    passwords.resize(channels.size(), "");
    size = channels.size();
    std::cout << "size " << size << std::endl;
    for (int i = 0; i < size; i++)
    {
        if (channels[i][0] == '#' || channels[i][0] == '&')
        {
            Channel *channel = _findChannel(channels[i]);
            if (!channel)
            {
                newChanel = new Channel(channels[i], client, passwords[i]);
                _channels.push_back(newChanel);
            }
            else
                channel->addMember(client, passwords[i]);
        }
        std::cout << "index : " << i << " size " << i << std::endl;
    }
    std::cout << "Out in Join parser Cmd" << std::endl;
}

void Server::_modeCmd(Client *client)
{
    if (client->commande_splited.size() < 3)
    {
        sendMsg(client->getFd(), client->getNick() + " "  + client->commande_splited[0] + " Not enough parameters\n");
        return ;
    }
    Channel *channel = _findChannel(client->commande_splited[1]);
    std::string mode = client->commande_splited[2];
    if (client->commande_splited.size() < 4 && (mode == "+o" || mode == "+k" || mode == "-o"))
    {
        sendMsg(client->getFd(), client->getNick() + " "  + client->commande_splited[0] + " Not enough parameters\n");
        return ;
    }
    if (!channel)
    {
        sendMsg(client->getFd(), client->getNick() + " " + client->commande_splited[1] +   " :No such channel\n");
        return ;
    }
    if (mode == "+i" || mode == "-i" || mode == "+t" || mode == "-t")
        channel->setModes(mode, client, "");
    else if (mode == "+o" || mode == "-o")
    {
        Client *Newclient = findClientByNick(client->commande_splited[3]);
        if (!Newclient)
            sendMsg(client->getFd(), client->getNick() + " " + client->commande_splited[3] +   " :No such user\n");
        else
            channel->setModes(mode, client, client->commande_splited[3]);
    }
    else if (mode == "+k")
        channel->setModes(mode, client, client->commande_splited[3]);
    else if (mode == "-k")
        channel->setModes(mode, client, "");
    else 
        sendMsg(client->getFd(), client->getNick() + " " + mode + " :is unknown mode char to me\n");
}


void Server::_topicCmd(Client *client) 
{
    int n;
    std::string topic;
    if (client->commande_splited.size() < 2)
    {
        sendMsg(client->getFd(), client->getNick() + " "  + client->commande_splited[0] + " Not enough parameters\n");
        return ;
    }
    n = client->commande_splited.size() == 2 ? 1 : 0;
    topic = client->commande_splited.size() == 2 ? "" : client->commande_splited[2];
    Channel *channel = _findChannel(client->commande_splited[1]);
    if (!channel)
    {
        sendMsg(client->getFd(), client->getNick() + " " + client->commande_splited[1] +   " :No such channel\n");
        return ;
    }
    channel->setTopic(topic, client, n);
}

void Server::_inviteCmd(Client *client)
{
    if (client->commande_splited.size() < 3)
    {
        sendMsg(client->getFd(), client->getNick() + " "  + client->commande_splited[0] + " Not enough parameters\n");
        return ;
    }
    Channel *channel = _findChannel(client->commande_splited[2]);
    if (!channel)
    {
        sendMsg(client->getFd(), client->getNick() + " " + client->commande_splited[2] +   " :No such channel\n");
        return ;
    }
    channel->addInvited(client->commande_splited[1], client);
}

void Server::_partCmd(Client *client)
{
    if (client->commande_splited.size() < 2)
    {
        sendMsg(client->getFd(), client->getNick() + " "  + client->commande_splited[0] + " Not enough parameters\n");
        return ;
    }
    std::vector<std::string> channels = joinCmdParser(client->commande_splited[1]);
    std::vector<std::string> raisons;
    if (client->commande_splited.size() > 2)
        raisons = joinCmdParser(client->commande_splited[2]);
    raisons.resize(channels.size(), "");
    for (size_t i = 0; i < channels.size(); i++)
    {
        Channel *channel = _findChannel(channels[i]);
        if (!channel)
        {
            sendMsg(client->getFd(), client->getNick() + " " + channels[i] +   " :No such channel\n");
            return ;
        }
        else
            channel->removeMember(client, raisons[i]);
    }
}