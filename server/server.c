#include "server.h"
#include "../methods.h"  // 包含Sendto,Recvform等包裹函数。
#include "../Transfer.h" // 发送模块
#include "../File_recv.h"// 接收模块
#include "check_md5.h"   // 验证md5模块
#include "show_dir.h"    // 显示目录模块

int server_socket;
struct sockaddr_in server_addr, client_addr;
socklen_t clen = sizeof(client_addr);

// 创建套接字，绑定到熟知地址上
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

// 等待握手
int WaitShakeHands(char *message, FILE **fp)
{
    printf("Waiting request ...\n");
    Packet fnpack;
    Recvfrom(server_socket, (char *)&fnpack, sizeof(Packet), 0, (struct sockaddr *)&client_addr, &clen);
    bzero(message, FILE_NAME_MAX_SIZE+1);
    strncpy(message, fnpack.data, fnpack.dataLength>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:fnpack.dataLength);
    // dataID==-1 表示下载请求
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
    }// dataID == -2 表示显示目录请求
    else if( fnpack.dataID == -2)
    {
        Sendto(server_socket, (char *)&fnpack, sizeof(Packet), 0, (struct sockaddr *)&client_addr, clen);
        return 2;
    }// dataID == -3 表示上传请求
    else if( fnpack.dataID == -3)
    {
        // 先验证md5,若存在同md5文件，则启动秒传
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
        // req_num 对应请求的dataID：1表示下载，2表示显示目录，3表示上传。若req_num<0,则无须后续传输。
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
