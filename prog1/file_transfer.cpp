#include "file_transfer.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <chrono>
#include <regex>
#include <vector>

FileTransfer::FileTransfer(){};

FileTransfer::FileTransfer(const std::string &data): data(data), isEmpty(true)
{
    
}

std::string stringProcesing2(std::string &str)
{
    if(str == "#")
    {
        return str;
    }
    int sum = 0;
    for(auto ch: str)
    {    
        if(ch == 'K' || ch == 'B')
        {
            sum += 0;
        }
        else
        {
            sum += ((int)ch - '0');
        }
        
    }
    return std::to_string(sum);
}

bool checkInput1(std::string &str)
{
    if(str == "#")
    {
        return true;
    }
    auto regex_exp = std::regex("[0-9]+");

    if (!std::regex_match(str, regex_exp)) {
        std::cout << "Invalid character" << std::endl;
        return false;
    }

    constexpr int kStringSize = 64;
    if (str.size() > kStringSize) {
        std::cout << "You have exceeded the character limit" << std::endl;
        std::cout << "Extra characters removed" << std::endl;
        str.resize(kStringSize);
    }
    return true;
}

std::string stringProcessing1(std::string &str) 
{
    if(str == "#")
    {
        return str;
    }
    std::vector<char> v;
    std::sort(rbegin(str), rend(str));

    for(auto ch: str)
    {    
        if((int)ch%2 == 0 && ch != '0')
        {
            v.push_back('K');
            v.push_back('B');
        }
        else
        {
           v.push_back(ch);
        }
    }
    
    std::string newStr(v.begin(), v.end());
    return newStr;
}
void FileTransfer::sendShMem(std::string &data)
{
    key_t key = ftok("../main.cpp",0);
    int shmid = shmget(key, data.size(), 0666|IPC_CREAT|IPC_EXCL);
    char* shString = (char*) shmat(shmid, NULL, 0);
    strcpy(shString, data.c_str());
    shmdt(shString);

    key = ftok("../file_transfer.cpp",0);
    shmid = shmget(key, sizeof(size_t), 0666|IPC_CREAT|IPC_EXCL);
    size_t* size = (size_t*) shmat(shmid, NULL, 0);
    *size = data.size();
    shmdt(size);  
}

std::string FileTransfer::getShMem()
{
    key_t key = ftok("../file_transfer.cpp", 0);
    int shmid = shmget(key , sizeof(size_t), 0);
    size_t* size = (size_t*)shmat(shmid, NULL, 0);
    size_t localSize = *size;
    shmdt(size);
    shmctl(shmid,IPC_RMID,NULL);

    key = ftok("../main.cpp", 0);
    shmid = shmget(key , (int)localSize, 0);
    char* shString = (char*)shmat(shmid, NULL, 0);
    std::string str(shString);
    shmdt(shString);
    shmctl(shmid,IPC_RMID,NULL);
    
    return str;
}

void FileTransfer::sendSocket()
{
    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) 
    {
        std::cout << "Error establishing socket..." << std::endl;
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNum);

    if (connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
    {
        std::cout << "Connection error to program #2, port number: " << portNum << std::endl;
    }
    
    send(client, dataForProg2.c_str(),dataForProg2.size() + 1 , 0);
    
    close(client);
}


void FileTransfer::sendResource()
{
    std::cout << "Enter numbers no more than 64 characters, or enter # to end the connection" << std::endl;

    do
    {  
        std::cin >> data;
    }
    while (!checkInput1(data));
    system("clear");
    dataForResource = stringProcessing1(data);
    
    std::unique_lock<std::mutex> locker(m);
    cv.wait(locker, [&]{return !isEmpty;});
    FileTransfer::sendShMem(dataForResource);
    isEmpty = true;
    cv.notify_one(); 
}

void FileTransfer::sendProg2()
{ 
    // std::cout << "TH2" << std::endl;
    std::unique_lock<std::mutex> locker(m);
    cv.wait(locker, [&]{return isEmpty;});
    // std::cout << "start prog2" << std::endl;
    // std::this_thread::sleep_for(std::chrono::milliseconds(200));
  
    
    dataFromResource = FileTransfer::getShMem();
    if(dataFromResource != "#")
    {
        std::cout << dataFromResource << std::endl;
    }
    
    dataForProg2 = stringProcesing2(dataFromResource);
    
  
    FileTransfer::sendSocket();

    isEmpty = false;
       
}
FileTransfer::~FileTransfer(){};

