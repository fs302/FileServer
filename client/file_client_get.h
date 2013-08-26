#include <netinet/in.h>  // for sockaddr_in
#include <sys/types.h>   // for socket
#include <sys/socket.h>  // for socket
#include <stdio.h>       // for printf
#include <stdlib.h>      // for exit
#include <string.h>      // for bzero
#include <time.h>

#define SERVER_PORT 6001 
#define CLIENT_PORT 6155
#define BUFFER_SIZE 512 
#define FILE_NAME_MAX_SIZE 512
#define MAX_PACKET_NUM 256
#define IP "127.0.0.1"
#define DEFAULT_FILE "Bible.txt"

#ifndef PACKET_H
#define PACKET_H
typedef struct
{
    int dataID,dataLength;      // dataID for data Packet or ack Packet; dataLength = sizeof(data)
    int flag;                   // -1:ack; 0:not written; 1:written;
    char data[BUFFER_SIZE];
} Packet;
#endif


int initConnection();

int ShakeHands(char *file_name, FILE **fp);

int main(int argc, char *argv[]);
