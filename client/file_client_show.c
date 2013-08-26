#include "file_client_show.h"
#include "../methods.h"

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

int ShakeHands()
{
    Packet fnpack;
    fnpack.dataID = -2;
    strncpy(fnpack.data, NULL , 0);
    fnpack.dataLength = 0;
    fnpack.flag = -1;
    
    printf("Sending request ...\n");
    Sendto(client_socket,(char *)&fnpack, sizeof(Packet),0, (struct sockaddr *)&server_addr, slen);
    
    Packet ack;
    Recvfrom(client_socket, (char *)&ack, sizeof(Packet), 0, (struct sockaddr *)&server_addr, &slen);
    if (ack.dataID == -5){
        printf("Failed.\n");
        return -5;
    }
    else{
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
    int Nid = 0, timeo = 0;
    Packet fn;
    if(ShakeHands()>0)
    {
        printf("-----------------------------------------\n");
        for(;;){
            if (timeo = readable_timeo(client_socket,1,0) > 0)
            {
                Recvfrom(client_socket, (char *)&fn, sizeof(fn), 0, (struct sockaddr*)&server_addr, &slen);
                if ( strncmp(fn.data,"END",strlen("END"))==0)
                {
                    fn.flag = 0;
                    Sendto(client_socket, (char *)&fn, sizeof(fn), 0, (struct sockaddr*)&server_addr, slen);
                    break;
                }
                if (fn.dataID == Nid)
                {
                    printf("%s\n",fn.data);
                    Nid ++;
                    }
                fn.flag = 0;
                Sendto(client_socket, (char *)&fn, sizeof(fn), 0, (struct sockaddr*)&server_addr, slen);
            }

        }
    }
    printf("-----------------------------------------\n");
    return 0;
}
