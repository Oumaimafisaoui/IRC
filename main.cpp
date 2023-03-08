#include "server.hpp"

int is_valid(long port_num, std::string password, char *end)
{
    if ((port_num < 0 || port_num > 65536 || *end != '\0') || 
    (password.length() < 1 || password.length() > 9))
        return (0);
    return (1);
}

int main(int argc, char **argv)
{
    char *end; // the next alpha elem
    int port_num;
    std::string password;
    
    if (argc != 3)
    {
        std::cout << "invalid number of arguments" << std::endl;
        return (1);
    }
    port_num = strtol(argv[1], &end, 10);
    password = argv[2];
    if (is_valid(port_num, argv[2], end))
    {
        try
        {
            Server serv = Server(port_num, password);
        }
        catch(const std::exception& e)
        {
            std::cout << e.what() << '\n';
        }
    }
    else
    {
        std::cout << "Invalid port range" << std::endl;
        return (1);
    }
    return (0);
}