#include <netinet/in.h>  // for sockaddr_in
#include <sys/types.h>   // for socket
#include <sys/socket.h>  // for socket
#include <sys/select.h>  // for select
#include <unistd.h>      // for alarm      
#include <stdlib.h>      // for exit,rand
#include <string.h>      // for bzero
#include <stdio.h>       // for printf
#include <time.h>        // for clock

#define SERVER_PORT 5155
#define BUFFER_SIZE 512 
#define FILE_NAME_MAX_SIZE 512
#define MAX_WINDOW_SIZE 256 
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

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

int WaitShakeHands(char *file_name, FILE **fp);

int main(int argc, char *argv[]);
