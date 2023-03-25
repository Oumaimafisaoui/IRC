 // to accumalate all the arguments
    std::ostringstream oss;
    std::copy(client->commande_splited.begin(), client->commande_splited.end(), std::ostream_iterator<std::string>(oss, " "));
    std::string commands = oss.str();

    //print the vector

    // std::vector<std::string> splited;
    // std::string temp = "";
    size_t dots = commands.find_last_of(":");
    // int j= 0;
    // for (int i = 8; commands[i]; i++)
    // {
    //     if (commands[i] == ',')
    //     {
    //         splited.push_back(temp);
    //         temp = "";
    //     }
    //     else
    //         temp += commands[i];
    //     j = i;
    // }
    // if (pos != std::string::npos && commands[j + 1] != ':')
    // {
    //     splited.push_back(commands.substr(j+1, pos - (j+1)));
    // }

    std::vector<std::string> split;
    std::string command;
    size_t pos = 8;
    size_t end = 0;
    
    while((end = commands.find(",", pos)) != std::string::npos)
    {
            command  = commands.substr(pos, end - pos);
            split.push_back(command);
            pos = end + 1;
    }

    //FIX PROBLEM OF SPACES

    if (pos < commands.length())
    {
            command = commands.substr(pos, commands.length() - dots + 1);
            split.push_back(command);
    }

//print the vector
    for (size_t i = 0; i < split.size(); i++)
    {
        std::cout << split[i] << std::endl;
    }
    //find the message
    // std::string result;
    // std::copy(commands.begin(), commands.end(), std::back_inserter(result));
    // size_t pos = result.find_last_of(":");

    // if(pos != std::string::npos)
    // {
    //     std::string message = commands.substr(pos + 1, result.size() - (pos + 1));
        
    // }
    // else
    // {
    //     std::cout << "Error: no message found" << std::endl;
    // }

