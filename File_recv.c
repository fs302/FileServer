#include "File_recv.h"

//文件接收模块
int FileReceive(int sockfd, struct sockaddr_in from, FILE **fp) {
    Packet recvWindow[MAX_PACKET_NUM+1];
    int rvwd = 64; // 暂定接收窗口大小为固定的64
    int recvfile = 0, FileNotEnd = 1, Nid = 0, RecvBuf = 0;
    struct timeval Ntime,Ptime;
    socklen_t slen = sizeof(from);

    gettimeofday(&Ntime,NULL);
    Ptime = Ntime;
    while(FileNotEnd)
    {
        int i;
        for(i=0;i<rvwd;i++)
            bzero(&recvWindow[i], sizeof(Packet));
        // Recv packet whose ID in [Nid,Nid+rvwd-1]
        
        while( (recvfile < rvwd) && (readable_timeo(sockfd, 5,0)>0) )
        {
           Packet Contentpack, ack; 
           Recvfrom(sockfd,(char *)&Contentpack, sizeof(Packet),0, (struct sockaddr *)&from, &slen);
           RecvBuf += Contentpack.dataLength;
           gettimeofday(&Ntime,NULL);
           
           if (Contentpack.dataID>=Nid && Contentpack.dataID < Nid+rvwd){
               bzero(&ack,sizeof(Packet));
               ack.dataID = Contentpack.dataID + 1;
               ack.flag = -1;
               ack.dataLength = 0;
               strncpy(ack.data, NULL, 0);
               Sendto(sockfd, (char *)&ack, sizeof(Packet),0, (struct sockaddr *)&from, slen);
               int newflag = 1;
               for(i=0;i<recvfile;i++)
               {
                   if (Contentpack.dataID==recvWindow[i].dataID){
                       newflag = 0;
                       break;
                   }
               }
               if (newflag)
               {
                   printf("New pack:%d\n",Contentpack.dataID);
                   if (strncmp(Contentpack.data, "*EOF*", 5) == 0){
                       FileNotEnd = 0;
                       rvwd = recvfile;
                       break;
                    }
                   recvWindow[recvfile++] = Contentpack;
               }
           }
           else if (Contentpack.dataID<Nid){
               bzero(&ack,sizeof(Packet));
               ack.dataID = Contentpack.dataID + 1;
               ack.flag = -1;
               ack.dataLength = 0;
               strncpy(ack.data, NULL, 0);
               Sendto(sockfd, (char *)&ack, sizeof(Packet), 0,(struct sockaddr *)&from, slen);
           }
        }
        if (recvfile < rvwd)
        {
            printf("Receive timeout. stop connection.\n");
            break;
        }
        // Write Packet data
        int id = Nid;
        for(id = Nid;id < Nid+rvwd;id++)
        {
            for(i=0;i<rvwd;i++)
            {
                if ( (recvWindow[i].dataID == id) && (recvWindow[i].flag!=1) ){
                    int write_len = fwrite(recvWindow[i].data, sizeof(char), recvWindow[i].dataLength, *fp);
                    if (write_len < recvWindow[i].dataLength){
                        printf("Write failed.\n");
                        exit(1);
                    }
                    recvWindow[i].flag = 1;
                    break;
                }
            }
        }
        Nid += rvwd;
        recvfile = 0;
    }
    printf("Receive %d packets.\n",Nid);
    return 0;
}
