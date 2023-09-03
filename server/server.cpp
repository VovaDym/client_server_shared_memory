#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

bool checkInput2(std::string &str)
{
    if(str.size() < 2)
    {
        return false;
    }
    if((atoi(str.c_str()))%32 != 0)
    {
        return false;
    }
    return true;
}

int main()
{
    int client, server;
    int portNum = 1500;
    bool isExit = false;
    int bufsize = 1024;
    char buffer[bufsize];
    

    struct sockaddr_in server_addr;
    socklen_t size;

    client = socket(AF_INET, SOCK_STREAM, 0);

    if (client < 0) 
    {
        std::cout << "Error establishing socket..." << std::endl;
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(portNum);

    if ((bind(client, (struct sockaddr*)&server_addr,sizeof(server_addr))) < 0) 
    {
        std::cout << "Error binding connection, the socket has already been established..." << std::endl;
        return -1;
    }

    size = sizeof(server_addr);
    std::cout << "Listen for Program №1..." << std::endl;

    listen(client, 1);

    int clientCount = 1;
    
    do 
    {
        server = accept(client,(struct sockaddr *)&server_addr,&size);
        system("clear");
        if (server < 0) 
        {
            std::cout << "Error on accepting..." << std::endl;
        }      
        
        recv(server, buffer, bufsize, 0);
        std::string str(buffer);
        if(str == "#")
        {
            break;
        }
        else if(!checkInput2(str))
        {
            std::cout << "Data error" << std::endl;
        }
        else
        {
            std::cout << "Data received" << std::endl;
        }
        
    }while (!isExit);
    
    std::cout << "Connection terminated with IP " << inet_ntoa(server_addr.sin_addr) << std::endl;
    close(server);
    std::cout << "Program №1 has completed data transfer" << std::endl;
    return 0;
}