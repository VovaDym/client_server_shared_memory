#include <iostream>
#include <thread>
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main()
{
    FileTransfer fT1;
    
    while (fT1.data != "#")
    {
        std::thread th1(&FileTransfer::sendResource, &fT1);
        std::thread th2(&FileTransfer::sendProg2, &fT1);
        th1.join();
        th2.join();
    } 
    
}