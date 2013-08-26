#include "server.h"
#include "../methods.h"
#include "../Transfer.h"
#include "../File_recv.h"
#include "check_md5.h"
#include "show_dir.h"

int server_socket;
struct sockaddr_in server_addr, client_addr;
socklen_t clen = sizeof(client_addr);

int initConnection()
{
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT); // SERVER_PORT 5155

    server_socket = socket(PF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0)
    {
        printf("Create Server Socket Failed.\n");exit(1);
    }
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        printf("Server Bind Port:%d Failed!\n", SERVER_PORT);exit(1);
    }
    return 0;
}

int WaitShakeHands(char *message, FILE **fp)
{
    printf("Waiting request ...\n");
    Packet fnpack;
    Recvfrom(server_socket, (char *)&fnpack, sizeof(Packet), 0, (struct sockaddr *)&client_addr, &clen);
    bzero(message, FILE_NAME_MAX_SIZE+1);
    strncpy(message, fnpack.data, fnpack.dataLength>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:fnpack.dataLength);
    if (fnpack.dataID == -1)
    {
        printf("Request File name: %s\n",message);
        *fp = fopen(message, "rb");
        if (NULL == *fp)
        {
            printf("File:\t %s is not found.\n",message);
            fnpack.dataID = -5;
            fnpack.dataLength = 0;
            strncpy(fnpack.data, NULL, 0);
            Sendto(server_socket, (char *)&fnpack, sizeof(Packet), 0, (struct sockaddr *)&client_addr, clen);
            return -5;
        }
        printf("File:\t %s Open.\n",message);
        fnpack.dataID = -1;
        fnpack.dataLength = 0;
        strncpy(fnpack.data, NULL, 0);
        Sendto(server_socket, (char *)&fnpack, sizeof(Packet), 0, (struct sockaddr *)&client_addr, clen);
        return 1;
    }
    else if( fnpack.dataID == -2)
    {
        Sendto(server_socket, (char *)&fnpack, sizeof(Packet), 0, (struct sockaddr *)&client_addr, clen);
        return 2;
    }
    else if( fnpack.dataID == -3)
    {
        if (check_md5(message)==1)
        {
            fnpack.dataID = -5;
            fnpack.dataLength = 0;
            strncpy(fnpack.data, NULL, 0);
            Sendto(server_socket, (char *)&fnpack, sizeof(Packet), 0, (struct sockaddr *)&client_addr, clen);
            return -5;
        }
        else{
            fnpack.dataID = 0; // Wait for packet 0
            fnpack.dataLength = 0;
            fnpack.flag = -1;
            strncpy(fnpack.data,NULL, 0);
            Sendto(server_socket, (char *)&fnpack, sizeof(Packet), 0, (struct sockaddr *)&client_addr, clen);
            return 3;
        }
    }
}

int main(int argc, char *argv[])
{
    initConnection();
    for(;;)
    {
        char message[BUFFER_SIZE];
        FILE *fp = NULL;
        int req_num = 0;
        req_num = WaitShakeHands(message, &fp);
        if ( req_num==1 )
        {
            Packet req;
            Recvfrom(server_socket, (char *)&req, sizeof(Packet),0, (struct sockaddr *)&client_addr, &clen);
            if ( req.dataID == 0 && req.flag ==-1)
                Transfer(server_socket, client_addr,&fp);
        }
        else if (req_num == 2)
        {
            Packet req;
            Recvfrom(server_socket, (char *)&req, sizeof(Packet),0, (struct sockaddr *)&client_addr, &clen);
            if ( req.dataID == 0 && req.flag ==-1)
                Show_dir(server_socket, client_addr);
        }
        else if (req_num == 3)
        {
            char file_name[BUFFER_SIZE],md5sum[33]={'\0'};
            int filesize;
            sscanf(message,"%s\t%d\t%s",file_name,&filesize,md5sum);
            fp = fopen(file_name, "wb");
            if (NULL == fp){
                printf("File:\t %s can not open to write.\n",file_name);exit(1);}
            printf("Begin Recvfile.\n");
            FileReceive(server_socket, client_addr,&fp);
            fclose(fp);
        }
        printf("Finished.\n");
    }
    return 0;
}
