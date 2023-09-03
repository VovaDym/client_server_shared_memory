#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#pragma once

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


const int bufsize = 1024;


class FileTransfer
{
public: 
    FileTransfer(); 
    FileTransfer(const std::string &data);
    ~FileTransfer();
    std::string data;
    std::string dataForResource;
    std::string dataFromResource;
    std::string dataForProg2;
    
    int client, server;
    int portNum = 1500;
    
    char* buffer[bufsize];
    struct sockaddr_in server_addr;
    std::mutex m;
    std::condition_variable cv;
    bool isEmpty;
    std::string getShMem();
    void sendShMem(std::string &data);
    void sendResource ();
    void sendProg2();   
    
    void sendSocket();
    
   
    
    

private:
   

};

#endif