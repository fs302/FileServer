/* receiver.c */
#include "file_client_get.h"
#include "../methods.h"
#include "../File_recv.h"

int client_socket;
struct sockaddr_in server_addr, client_addr;
socklen_t slen = sizeof(server_addr);

int initConnection()
{
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);
    client_addr.sin_port = htons(CLIENT_PORT); // CLIENT_PORT 5155
    
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        printf("Create Client Socket Failed.\n");
        exit(1);
    }
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if ( inet_aton(IP, &server_addr.sin_addr) == 0){
        printf("Server IP Address Error.\n");
        exit(1);
    }
    server_addr.sin_port = htons(SERVER_PORT); 
    return 0;
}

int ShakeHands(char *file_name, FILE **fp)
{
    // Fill in file name to filenamePacket
    Packet fnpack;
    fnpack.dataID = -1;
    strncpy(fnpack.data, file_name, strlen(file_name));
    fnpack.dataLength = strlen(file_name);
    fnpack.flag = -1;
    
    printf("Sending request ...\n");
    Sendto(client_socket,(char *)&fnpack, sizeof(Packet),0, (struct sockaddr *)&server_addr, slen);
    
    Packet ack;
    Recvfrom(client_socket, (char *)&ack, sizeof(Packet), 0, (struct sockaddr *)&server_addr, &slen);
    if (ack.dataID == -2){
        printf("File not found.\n");
        return -2;
    }
    else{
        *fp = fopen(file_name, "wb");
        if (NULL == *fp){
            printf("File:\t %s can not open to write.\n",file_name);exit(1);
        }
        ack.dataID = 0;
        ack.dataLength = 0;
        strncpy(ack.data, NULL, 0);
        ack.flag = -1;
        Sendto(client_socket,(char *)&ack, sizeof(Packet),0, (struct sockaddr *)&server_addr, slen);
    }
    return 1;
}

int main(int argc, char *argv[])
{
    initConnection();
    
    char file_name[FILE_NAME_MAX_SIZE+1];
    bzero(file_name, FILE_NAME_MAX_SIZE+1);
    //strncpy(file_name,DEFAULT_FILE,strlen(DEFAULT_FILE));
    if (argc>1){
        strncpy(file_name,argv[1],strlen(argv[1]));
    }
    FILE *fp = NULL;
    if (ShakeHands(file_name, &fp)>0){
        printf("Receving...\n"); 
        struct timeval start,finish;
        gettimeofday(&start,NULL);
        FileReceive(client_socket,server_addr, &fp);
        fclose(fp);
        gettimeofday(&finish,NULL);
        freopen("/dev/tty","w",stdout);
        printf("Receive File:\t %s From Server [%s] Finished.\n", file_name, IP);
        double duration = (double)((finish.tv_sec-start.tv_sec)*1000000.0+finish.tv_usec-start.tv_usec)/1000000.0;
        printf("Duration: %.3lf sec\n",duration);
    }
    
    return 0;
}
