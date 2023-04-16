#include "server.hpp"
#include "sys/socket.h"
#include <string>
#include <sys/_types/_size_t.h>
#include <sys/poll.h>

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


void Server::_botCmd(Client *client)
{
    if(client->commande_splited.size() < 2)
    {
        sendMsg(client->getFd(), ":" + client->get_nick_adresse(client) + " NOTICE " + client->getNick() + " :" + "BOT :Welcome to IRC Bot ! Please insert /bot Quote or /bot Joke or /bot Time and enjoy!\r\n");
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
        sendMsg(client->getFd(), ":" + client->get_nick_adresse(client) + " NOTICE " + client->getNick() + " :" + " ,Here is your Quote for today: " + quote + "\r\n");
    }
    else if (client->commande_splited[1] == "Joke")
    {
        std::ifstream file("jokes.txt");
        std::string quote;
        int line = rand() % 10;
        for (int i = 0; i < line; i++)
            std::getline(file, quote);
        file.close();
          sendMsg(client->getFd(), ":" + client->get_nick_adresse(client) + " NOTICE " + client->getNick() + " :" + " ,Here is your Joke for today: " + quote + "\r\n");
    }
    else if (client->commande_splited[1] == "Time")
    {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        std::ostringstream oss;
        oss << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec;
        std::string time = oss.str();
          sendMsg(client->getFd(), ":" + client->get_nick_adresse(client) + " NOTICE " + client->getNick() + " :" + " Here is the time: " + time + "\r\n");

    }
    else
    {
          sendMsg(client->getFd(), ":" + client->get_nick_adresse(client) + " NOTICE " + client->getNick() + " :" + " BOT :Wrong command!\r\n");
        return ;
    }
    
}



Server::~Server(void)
{
    std::cout << "Distructor for server is called" << std::endl;
    _freeAll();
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
            this->poll_vec.erase(i);
            // clearEpoll(client->getFd());
            delete client;
            std::cout << "client went away!!" << std::endl;
            return ;
        }
        else
        {
            std::string buff = this->buffer;
            client->buff_client.append(buff);
            std::size_t pos = 0;
            while ((pos = client->buff_client.find("\r\n", pos)) != std::string::npos) 
            { 
                client->buff_client.replace(pos, 2, "\n");
            }
            if (client->buff_client.find('\n') != std::string::npos && client->buff_client.size() > 1)
            {
                if (!client->checkIsRegister())
                    client_not_connected(client);
                else
                    client_connected(client);
                client->buff_client.clear();
            } 
        }
    }
}


void Server::client_connected(Client *client)
{
   std::vector<std::string> command_split;
   std::string  str = client->buff_client;
   std::string temp = "";
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] == ' ')
        {
            if (temp[0] != '\n')
                command_split.push_back(temp);
            temp = "";
            while (str[i] && str[i + 1] == ' ')
                i++;
        }
        else
            temp += str[i];
    }
    if (temp[0] != '\0' && temp[0] != '\n')
        command_split.push_back(temp);
    std::string m = command_split[command_split.size() - 1];
    if (m[m.size() - 1] == '\n')
        m = m.substr(0, m.size() - 1);
    command_split[command_split.size() - 1] = m;
    command_split = ft_parser(command_split);
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
        command_split.clear();
        std::string  str = message_split[k];
        std::string temp = "";
        for (size_t i = 0; i < str.length(); i++)
        {
            if (str[i] == ' ')
            {
                if (temp[0] != '\n')
                    command_split.push_back(temp);
                temp = "";
                while (str[i] && str[i + 1] == ' ')
                    i++;
            }
            else
                temp += str[i];
        }
        if (temp[0] != '\0' && temp[0] != '\n')
            command_split.push_back(temp);
        
        // std::cerr << "{";
        // for (unsigned int i = 0;i < command_split.size();++i)
        //     std::cerr << command_split[i] << " ";
        // std::cerr << "}\n";

        this->_command_splited = command_split;
        if (_isNotChannelCmd(command_split)) {
        client->setCommand(command_split);
        if (!client->execute())
        {
            std::cout << "Client execute" << std::endl;
            command_split.clear();
            this->client->buff_client.clear();
            message_split.clear();
            break;
        }
        }
        command_split.clear();
        this->client->buff_client.clear();
        ++k; 
   }
   std::cout << "out" << std::endl;
   std::cout << client->buff_client.size() << " buffer size " << std::endl;
   return; 
}

std::vector<std::string> Server::ft_parser(std::vector<std::string> params)
{
    std::vector<std::string> ret;
    size_t n = 0;
    std::string rest = "";
    if (params[0][0] == ':')
        n = 1;
    for (size_t i = n; i < params.size(); i++)
    {
        ret.push_back(params[i]);
        if (ret.size() == 15)
        {
            i++;
            while (i < params.size())
            {
                rest += params[i];
                if (i != params.size() + 1)
                    rest += " ";
                i++;
            }
            if (rest != "")
                ret.push_back(rest);
            break ;
        }
    }
    return ret;
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
                    fcntl(client_fd, F_SETFL, O_NONBLOCK);
                    if (client_fd < 0)
                        throw std::runtime_error("Problem in accept client: ");
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
                if (this->clients.find(current.fd) != this->clients.end())
                {
                    size_t pos = this->client->buff_client.find("\n");
                    if ( pos != std::string::npos) {
                        this->client->buff_client =  this->client->buff_client.substr(pos + 1,  this->client->buff_client.size());
                    }
                }
            }
        }
    }
}

void Server::sendMsg(int fd, std::string msg)
{
     int ret = send(fd, msg.c_str(), msg.length(), 0);
    if (ret == -1 && (errno == EPIPE || errno == ECONNRESET)) {
        close(fd);
        fd = -1;
    }
}

bool Server::_isNotChannelCmd(std::vector<std::string> command_splited)
{
    std::cout << "HEEERE IS COMMAND[0]" <<  this->_command_splited[0] << std::endl;
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
    if(clients.size() == 0)
        return false;
    for (std::map<int, Client*>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
    {
        if (it->second->getNick() == nick && it->second->getNick() != "")
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
}


void Server::_execute_commands(Client *client) 
{
    std::cout << "commande_splited[0]: " << ":" << client->commande_splited[0] << ":" << std::endl;
    if (client->commande_splited[0] == "JOIN" || client->commande_splited[0] == "join")
        _joinCmd(client);
    else if (client->commande_splited[0] == "MODE" || client->commande_splited[0] == "mode")
        _modeCmd(client);
    else if (client->commande_splited[0] == "PRIVMSG" || client->commande_splited[0] == "privmsg")
        _privMsgCmd(client, true);
    else if (client->commande_splited[0] == "NOTICE" || client->commande_splited[0] == "notice")
        _privMsgCmd(client, false);
    else if (client->commande_splited[0] == "TOPIC" || client->commande_splited[0] == "topic")
        _topicCmd(client);
    else if (client->commande_splited[0] == "INVITE" || client->commande_splited[0] == "invite")
        _inviteCmd(client);
    else if (client->commande_splited[0] == "BOT" || client->commande_splited[0] == "bot")
        _botCmd(client);
    else if (client->commande_splited[0] == "PART" || client->commande_splited[0] == "part")
        _partCmd(client);
    else if (client->commande_splited[0] == "KICK" || client->commande_splited[0] == "kick")
        _kickCmd(client);
    else if (client->commande_splited[0] == "QUIT" || client->commande_splited[0] == "quit")
        _quitCmd(client);
    else if(client->commande_splited[0] == "WALLOPS" || client->commande_splited[0] == "wallops")
        _wallopsCmd(client);
    else if(client->commande_splited[0] == "OPER" || client->commande_splited[0] == "oper")
        _operCmd(client);
    else if(client->commande_splited[0] == "PONG" || client->commande_splited[0] == "PING")
        return ;
    else
        sendMsg(client->getFd(), ":IRC 421 " + client->getNick() + " " + client->commande_splited[0]  + " :Unknown command\r\n");
}


void Server::_wallopsCmd(Client *client)
{
    std::vector<std::string> message_vec;

    for(size_t i = 1; i < client->commande_splited.size(); i++)
    {
        message_vec.push_back(client->commande_splited[i]);
    }
    std::string message = "";
    for(size_t i = 0; i < message_vec.size(); i++)
    {
        message += message_vec[i];
        if (i != message_vec.size() - 1)
            message += " ";
    }
    if(client->commande_splited.size() < 2)
    {
        std::string errorMessage = ":" + client->get_nick_adresse(NULL) + "  412  " + (client->getNick().empty() ? "*" : client->getNick()) + " " + ":No text to send\r\n";
        sendMsg(client->getFd(), errorMessage);
        return;
    }
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second->get_isoperator() == false)
        {
            sendMsg(it->second->getFd(), message);
        }
    }
    return ;
}


std::string Server::get_message(Client *client)
{
    std::vector<std::string> message_vec;
    for(size_t i = 2; i < client->commande_splited.size(); i++)
    {
        message_vec.push_back(client->commande_splited[i]);
    }
    std::string message = "";
    for(size_t i = 0; i < message_vec.size(); i++)
    {
        message += message_vec[i];
        if (i != message_vec.size() - 1)
            message += " ";
    }
    size_t pos = message.find(':');
    if (pos != std::string::npos)
        message = message.substr(pos + 1);

    return (message);
}

void Server::find_channel_and_sendmsg1(Client *client, std::string &target, std::string &message, bool error)
{
    Channel *tmp = find_channel(target);
    std::string flag_com;

    error == true ? (flag_com = "PRIVMSG") : flag_com = "NOTICE";

    if (!tmp && error)
    {
        sendMsg(client->getFd(), ":" + client->get_nick_adresse(NULL)+ " 403 " + (client->getNick().empty() ? "*" : client->getNick()) + " " + target + " :No such channel\r\n");
        return ;
    }
    else if (!tmp && !error)
        return ;
    else
    {
        std::set<Client*> clients = tmp->getClients(); 
        tmp->sendToOthers(":" + client->get_nick_adresse(NULL) + " " + flag_com + " " + tmp->getChannelName() + " :" + message + "\r\n", client->getFd());
    }
    return ;
}


void Server::find_client_and_sendmsg1(Client *client, std::string &target, std::string &message, bool error)
{
    Client *tmp = find_client(target);
    std::string flag_com;  

    error == true ? (flag_com = "PRIVMSG") : flag_com = "NOTICE";

    if (tmp != NULL)
    {
        sendMsg(tmp->getFd(), ":" + tmp->get_nick_adresse(tmp) + " " + flag_com + " " + tmp->getNick() + " :" + message + "\r\n");
        return ;
       
    }
    else if (!tmp)
    {
        if (error == 1)
        {
            sendMsg(client->getFd(), ":" + client->get_nick_adresse(NULL) + " 401 " + (client->getNick().empty() ? "*" : client->getNick()) + " " + " :No such nick/Channel\r\n");
            return ;
        }
        else
            return ;
    }
    return ;
}

void Server::sendmessage(std::string &message, Client* client, bool error)
{
    std::vector<std::string> targets;

    if (client->commande_splited[1].find(',') != std::string::npos)
    {
        std::stringstream ss(client->commande_splited[1]);
        std::string token;
        while (std::getline(ss, token, ','))
        {
            targets.push_back(token);
        }
        for (size_t i = 0; i < targets.size(); i++)
        {
            if (targets[i][0] == '#')
                find_channel_and_sendmsg1(client, targets[i], message, error);
            else
                find_client_and_sendmsg1(client, targets[i], message, error);
        }
    }
    else
    {
        if (client->commande_splited[1][0] == '#')
            find_channel_and_sendmsg1(client,client->commande_splited[1], message, error);
        else
            find_client_and_sendmsg1(client, client->commande_splited[1], message, error);
    }
}

void Server::_privMsgCmd(Client *client, bool error)
{
    std::string flag_com;  

    error == true ? (flag_com = "PRIVMSG") : flag_com = "NOTICE";

    if (client->commande_splited.size() < 2)
    {
        sendMsg(client->getFd(), ":" + client->getHost()+ " 411 " + (client->getNick().empty() ? "*" : client->getNick()) + " " + " :No recipient given " + flag_com + " " + "\r\n");
        return;
    }
    else if (client->commande_splited.size() == 2)
    {
        sendMsg(client->getFd(), ":" + client->getHost()+ " 412 " + (client->getNick().empty() ? "*" : client->getNick()) + " " + " :No text to send " + flag_com + " " + "\r\n");
        return;
    }
    std::string message = get_message(client);
    std::cout << message << std::endl;
    sendmessage(message, client, error);
}


void Server::_joinCmd(Client *client)
{
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
    for (int i = 0; i < size; i++)
    {
        if (channels[i][0] == '#' || channels[i][0] == '&')
        {
            Channel *channel = _findChannel(channels[i]);
            if (!channel)
            {
                newChanel = new Channel(channels[i], client);
                _channels.push_back(newChanel);
            }
            else
                channel->addMember(client, passwords[i]);
        }
        else {
            sendMsg(client->getFd(), ":IRC 403 " + client->getNick() + " " + channels[i] + " :No such channel\r\n");
        }
    }
}

void Server::_modeCmd(Client *client)
{
    if (client->commande_splited.size() < 3)
    {
         sendMsg(client->getFd(), ":IRC 461 " + client->getNick() + " MODE :Not enough parameters\r\n");
        return ;
    }
    Channel *channel = _findChannel(client->commande_splited[1]);
    std::string mode = client->commande_splited[2];
    if (client->commande_splited.size() < 4 && (mode == "+o" || mode == "+k" || mode == "-o"))
    {
        sendMsg(client->getFd(), ":IRC 461 " + client->getNick() + " MODE :Not enough parameters\r\n");
        return ;
    }
    if (!channel)
    {
        sendMsg(client->getFd(), ":IRC 403 " + client->getNick() + " " + client->commande_splited[1] + " :No such channel\r\n");
        return ;
    }
    if (mode == "+i" || mode == "-i" || mode == "+t" || mode == "-t")
        channel->setModes(mode, client, "");
    else if (mode == "+o" || mode == "-o")
    {
        Client *Newclient = findClientByNick(client->commande_splited[3]);
        if (!Newclient)
            sendMsg(client->getFd(), ":IRC 401 " + client->commande_splited[1] + " " + client->getNick() + " :No such nick/channel\r\n");
        else
            channel->setModes(mode, client, client->commande_splited[3]);
    }
    else if (mode == "+k")
        channel->setModes(mode, client, client->commande_splited[3]);
    else if (mode == "-k")
        channel->setModes(mode, client, "");
    else if (mode == "+sn")
        return; 
    else 
        sendMsg(client->getFd(), ":IRC 472 " + client->getNick() + " " + mode.substr(1, 2) + " :is unknown mode char to me\r\n");
}


void Server::_topicCmd(Client *client) 
{
    int n;
    std::string topic = "";
    if (client->commande_splited.size() < 2)
    {
        sendMsg(client->getFd(), ":IRC 461 " + client->getNick() + " TOPIC :Not enough parameters\r\n");
        return ;
    }
    n = client->commande_splited.size() == 2 ? 1 : 0;
    if (!n)
    {
        if (client->commande_splited[2][0] == ':')
        {
            for (size_t i = 2; i < client->commande_splited.size(); i++)
            {
                if (i == 2)
                    topic += client->commande_splited[i].substr(1, client->commande_splited[i].length());
                else
                    topic += client->commande_splited[i];
                if (i + 1 != client->commande_splited.size())
                    topic += " ";
            }
        }
        else
            topic = client->commande_splited[2];
    }
    if (topic == " ")
        topic = "";
    Channel *channel = _findChannel(client->commande_splited[1]);
    std::cout << n << std::endl;
    if (!channel)
    {
        sendMsg(client->getFd(), ":IRC 403 " + client->getNick() + " " + client->commande_splited[1] + " :No such channel\r\n");
        return ;
    }
    channel->setTopic(topic, client, n);
}

void Server::_inviteCmd(Client *client)
{
    if (client->commande_splited.size() < 3)
    {
        sendMsg(client->getFd(), ":IRC 461 " + client->getNick() + " INVITE :Not enough parameters\r\n");
        return ;
    }
    Client *Newclient = findClientByNick(client->commande_splited[1]);
    if (!Newclient)
    {
        sendMsg(client->getFd(), ":IRC 401 " + client->commande_splited[2] + " " + client->getNick() + " :No such nick/channel\r\n");
        return ;
    }
    Channel *channel = _findChannel(client->commande_splited[2]);
    if (!channel)
    {
        sendMsg(client->getFd(), ":IRC 403 " + client->getNick() + " " + client->commande_splited[2] + " :No such channel\r\n");
        return ;
    }
    channel->addInvited(client->commande_splited[1], client, Newclient);
}

void Server::_partCmd(Client *client)
{
    if (client->commande_splited.size() < 2)
    {
        sendMsg(client->getFd(), ":IRC 461 " + client->getNick() + " PART :Not enough parameters\r\n");
        return ;
    }
    std::vector<std::string> channels = joinCmdParser(client->commande_splited[1]);
    std::string raison("");
    if (client->commande_splited.size() > 2)
        raison = ":" + client->commande_splited[2];
    for (size_t i = 0; i < channels.size(); i++)
    {
        Channel *channel = _findChannel(channels[i]);
        if (!channel)
            sendMsg(client->getFd(), ":IRC 403 " + client->getNick() + " " + channels[i] + " :No such channel\r\n");
        else
        {
            channel->removeMember(client, raison);
            if (channel->getClients().size() == 0)
            {
                std::vector<Channel *>::iterator iterc = std::find(_channels.begin(), _channels.end(), channel);
                _channels.erase(iterc);
            }
        }
    }
}


void Server::_kickCmd(Client *client)
{
    std::string comment = "";
    size_t size = client->commande_splited.size();
    if (size < 3 || client->commande_splited[2] == ":")
    {
        sendMsg(client->getFd(), ":IRC 461 " + client->getNick() + " KICK :Not enough parameters\r\n");
        return ;
    }
    Channel *channel = _findChannel(client->commande_splited[1]);
    if (!channel)
    {
        sendMsg(client->getFd(), ":IRC 403 " + client->getNick() + " " + client->commande_splited[1] + " :No such channel\r\n");
        return ;
    }
    std::vector<std::string> users = joinCmdParser(client->commande_splited[2]);
    if (size > 3)
    {
        if (client->commande_splited[3][0] == ':')
            for (size_t i = 3; i < size; i++)
                comment += client->commande_splited[i] + " ";
        else
            comment = client->commande_splited[3];
    }
    for (size_t i = 0; i < users.size(); i++)
        channel->kickClient(client, users[i], comment);
}

void Server::_quitCmd(Client *client)
{
    std::string arg = "";
    size_t size = client->commande_splited.size();
    if (size > 1)
    {
        if (client->commande_splited[1][0] == ':')
        {
            for (size_t i = 1; i < size ; i++)
            {
                if (i == 1)
                    arg += client->commande_splited[i].substr(1, size);
                else
                    arg += client->commande_splited[i];
                if (i + 1 != size)
                    arg += " ";
            }
        }
        else {
            arg = client->commande_splited[1];
        }
    }
    else {
        arg = client->getNick();
    }
    for (size_t i = 0; i < _channels.size(); i++)
    {
        if (_channels[i]->isMember(client))
        {
            _channels[i]->clearMember(client);
            _channels[i]->sendToMembers(":" + client->get_nick_adresse(NULL) + " QUIT " + ":Quit: " + arg);
        }
    }
    sendMsg(client->getFd(), "ERROR : Closing Link 0.0.0.5\r\n");
    clients.erase(client->getFd());
    clearEpoll(client->getFd());
    delete client;
    std::cout << "client went away!!" << std::endl;
}

void Server::_operCmd(Client *client)
{
    std::string password("miros");
    if (client->commande_splited.size() < 3)
    {
        sendMsg(client->getFd(), ":IRC 461 " + client->getNick() + " KICK :Not enough parameters\r\n");
        return ;
    }
    if (client->commande_splited[2] != password)
    {
        sendMsg(client->getFd(), ":IRC 464 " + client->getNick() + " :Password incorrect\r\n");
        return ;
    }
    if (client->commande_splited[1] == client->getNick())
    {
        client->setOperatorStatus(true);
        sendMsg(client->getFd(), ":IRC 381 " + client->getNick() + " :You are now an IRC operator\r\n");
        sendMsg(client->getFd(), ":" + client->get_nick_adresse(NULL) + " " + "MODE +O\r\n");
    }
}

void Server::_freeAll()
{
    std::map<int, Client*>::iterator iter;
    for (iter = clients.begin(); iter != clients.end(); iter++)
        delete (*iter).second;
    for (size_t i = 0; i < _channels.size(); i++)
        delete _channels[i];
}

void Server::clearEpoll(int fd)
{
    for (std::vector<pollfd>::iterator iter = poll_vec.begin(); iter != poll_vec.end(); iter++)
    {
        if ((*iter).fd ==  fd)
        {
            poll_vec.erase(iter);
            break ;
        }
    }
}