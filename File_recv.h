#include "methods.h"

#define BUFFER_SIZE 512 
#define MAX_PACKET_NUM 256

#ifndef PACKET_H
#define PACKET_H
typedef struct
{
    int dataID,dataLength;      // dataID for data Packet or ack Packet; dataLength = sizeof(data)
    int flag;                   // -1:ack; 0:not written; 1:written;
    char data[BUFFER_SIZE];
} Packet;
#endif

int FileReceive(int sockfd, struct sockaddr_in from, FILE **fp) ;
