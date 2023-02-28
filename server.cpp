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
    struct pollfd ser_poll[200];

    std::memset(&sockaddr, 0, sizeof sockaddr);
  
    sockaddr.sin_family = FAMILY;
    sockaddr.sin_addr.s_addr = ADDR;
    sockaddr.sin_port = htons(port);

    socklen_t len = sizeof(sockaddr);
    int flag = 1;
    int end = FALSE;
    int new_sd;
    int nfd = 1;
    int current;
    int close_con;
    int compress = FALSE;
    char buffer[90];
    this->port = port;
    this->password = password;
    this->fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (this->fd < 0)
        throw Server::ProblemInFdServer();
    if(setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0)
        throw Server::ProblemInSockOpt();
    if(fcntl(this->fd,F_SETFL, O_NONBLOCK) < 0)
        throw Server::ProblemInFcntl();
    if(bind(this->fd,(struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
         throw Server::ProblemInBindServer();
    if (listen(this->fd, 32) < 0)
         throw Server::ProblemInlistenServer();


    std::memset(&ser_poll, 0, sizeof(ser_poll));
    ser_poll[0].fd = this->fd; //first monitor fd serv
    ser_poll[0].events = POLLIN; //monitor the input events
    int timeout = (3 * 60 * 1000);
    int i = 0;
    int j = 0;
    do
    {
       std::cout << "The poll function is listening..." << std::endl;
       int ret = poll(ser_poll, nfd, timeout);

       if (ret < 0) //poll failled
       {
            perror("poll");
            break ;
            
       }
       if(ret == 0) //TIME EXTENDED
       {
            perror("time");
            break;
       }
       current = nfd;
       for (i = 0; i < current ; i++)
       {
        //fds[0].revents contains the POLLIN flag, this indicates that input is available on fd1.
            if (ser_poll[i].revents == 0)
                continue; //no events have occurred on the associated file descriptor.
            if (ser_poll[i].revents != POLLIN)
            {
                 printf("  Error! revents = %d\n", ser_poll[i].revents);
                 end = TRUE;
                 break ; //no data in the fd
            }
            if (ser_poll[i].fd == this->fd)
            {
                  printf("  Listening socket is readable\n"); 
                  do
                  {
                    new_sd = accept(this->fd, NULL, NULL);
                    //attempt to read data from a non-blocking socket and no data is currently available
                    if (new_sd < 0)
                    {
                        if (errno != EWOULDBLOCK)
                        {
                            perror("accept");
                            end = TRUE;
                        }
                        break ;
                    }

                    std::cout << "new incoming connection" << std::endl;
                    ser_poll[nfd].fd = new_sd;
                    ser_poll[nfd].events = POLLIN;
                    nfd++;
                  } while(new_sd != -1);
            }
            else
            {
                std::cout << "Descriptor : " << ser_poll[i].fd << std::endl;
                close_con = FALSE;
                do
                {
                    ret  = recv(ser_poll[i].fd, buffer, sizeof(buffer), 0);
                    if (ret < 0)
                    {
                        if (errno != EWOULDBLOCK)
                            {
                                perror("  recv() failed");
                                close_con = TRUE;
                            }
                        break;
                    }
                    if (ret  == 0)
                    {
                        std::cout << "connection closed" << std::endl;
                        close_con = TRUE;
                        break;
                    }
                    len = ret;
                    std::cout << "Bytes received: " << len << std::endl;
                    ret = send(ser_poll[i].fd, buffer, len,0);
                    if (ret< 0)
                    {
                        perror("  send() failed");
                        close_con = TRUE;
                        break;
                    }
                } while (TRUE);
                if(close_con)
                {
                    close(ser_poll[i].fd);
                    ser_poll[i].fd = -1;
                    compress = TRUE;
                }
            }
       }
        if(compress)
        {
            compress = FALSE;
            for (i = 0 ; i < nfd; i++)
            {
                if (ser_poll[i].fd == -1)
                {
                    for (j = i; j < nfd; j++)
                    {
                        ser_poll[j].fd = ser_poll[j+1].fd;
                    }
                    i--;
                    nfd--;
                }
            }
        }
       }while(end == FALSE);

       for(i = 0; i < nfd; i++)
       {
        if(ser_poll[i].fd >= 0)
            close(ser_poll[i].fd);
       }
} 

    
    // int acce = accept(this->fd, (struct sockaddr *)&sockaddr, &len);
    // if(acce < 0)
    //     throw Server::ProblemInConnectServer();

    
    // char buffer[100];
    // int bytesRead = read(acce, buffer, 10);
    // if (bytesRead < 0)
    //         exit(1);
    // std::cout << "The message was:" << buffer;
    // std::string response = "Good talking to you\n";
    // send(acce, response.c_str(), response.size(), 0);
    // close(acce);
    // close(this->fd);