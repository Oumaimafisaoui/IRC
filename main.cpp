#include "server.hpp"

int is_valid(long port_num, char *end)
{
    if (port_num < 0 || port_num > 65536 || *end != '\0')
        return (0);
    return (1);
}

int main(int argc, char **argv)
{
    char *end; // the next alpha elem
    int port_num;
    if (argc != 3)
    {
        std::cout << "invalid number of arguments" << std::endl;
        return (1);
    }
    port_num = strtol(argv[1], &end, 10);
    if (is_valid(port_num, end))
    {
        try
        {
            Server serv = Server(port_num, argv[2]);
            std::cout << "All Cool\n";
        }
        catch(const std::exception& e)
        {
            std::cout << "Problem in irc!" << '\n';
            return (1);
        }
    }
    else
    {
        std::cout << "Invalid port range" << std::endl;
        return (1);
    }
    return (0);
}