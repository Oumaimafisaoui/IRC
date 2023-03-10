#include "server.hpp"



void Server::receive_message(std::vector<pollfd>::iterator i)
{
    int len;
    this->message = "";

    len = recv(i->fd, this->buffer, 500, 0);
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
            std::cout << message << std::endl;
        }
    }
   
   // Split message with delimiter "\n" and store it in a vector
   std::vector<std::string> message_parts;
   std::size_t pos = 0;
   std::size_t end_pos = 0;
   while ((end_pos = message.find("\n", pos)) != std::string::npos)
   {
       std::string part = message.substr(pos, end_pos - pos);
       message_parts.push_back(part);
       pos = end_pos + 1;
   }
   // If there is any remaining message after the last delimiter, add it to the vector
   if (pos < message.length())
   {
       std::string part = message.substr(pos, message.length() - pos);
       message_parts.push_back(part);
   }
   
   // Do something with the message parts stored in the vector
   for (const std::string& part : message_parts)
   {
       // Process each message part
       // ...
   }
}
