#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include "../methods.h"

#define BUFFER_SIZE 512 
#define FILE_NAME_MAX_SIZE 512
#define MAX_WINDOW_SIZE 256 
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))
#define DEFAULT_DIR "/home/fs302/file_server/server"

#ifndef PACKET_H
#define PACKET_H
typedef struct
{
    int dataID,dataLength;      // dataID for data Packet or ack Packet; dataLength = sizeof(data)
    int flag;                   // -1:ack; 0:not written; 1:written;
    char data[BUFFER_SIZE];
} Packet;
#endif

int Show_dir(int sockfd, struct sockaddr_in to);

