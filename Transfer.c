#include "Transfer.h"

int Transfer(int sockfd, struct sockaddr_in to, FILE **fp)
{
    Packet sendWindow[MAX_WINDOW_SIZE];
    int cwnd = 1; // Initialize the sending window size
    int ssthresh = 65535;
    int recvack = 0, FileNotEnd = 1, file_block_length = 0, Nid = 0;
    char buffer[BUFFER_SIZE];
    int reSendNum = 0;
    socklen_t clen = sizeof(to);
    printf("Begin transfer.\n");
    while(FileNotEnd)
    {
        int i;
        int losepack = 0;
        // Step1: Fill in Packets
        for(i = 0;i < cwnd;i++)
        {
            bzero(&buffer, BUFFER_SIZE);
            file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, *fp);
            /**/
            //printf("%s\n",buffer);
            /**/
            if (file_block_length < 0){
                printf("Read file buffer error.\n");
                return -1;
            }
            else if (file_block_length == 0){
                FileNotEnd = 0;
                Packet EndSig;
                EndSig.dataID = Nid++;
                strncpy(EndSig.data, "*EOF*", 5);
                EndSig.dataLength = strlen(EndSig.data);
                EndSig.flag = 0;
                sendWindow[i] = EndSig;
                cwnd = i+1;
                break;
            }
            Packet Contentpack;
            Contentpack.dataID = Nid++;
            Contentpack.dataLength = file_block_length;
            Contentpack.flag = 0;
            memcpy(Contentpack.data, buffer, BUFFER_SIZE);
            sendWindow[i] = Contentpack;
        }
        // Step2: Sending & Recv ack
        recvack = 0;
        int MaxACK = 0;
        while(recvack<cwnd)
        {
            printf("Sending %d packets.\n",cwnd);
            //printf("Sstresh:%d\n",ssthresh); 
            for(i = 0;i < cwnd;i++)
            {
                if (sendWindow[i].flag == 0){
                    Sendto(sockfd, (char *)&sendWindow[i], sizeof(Packet),0, (struct sockaddr*)&to,clen);
                    if (losepack==1)
                        reSendNum++;
                        //printf("reSending ID:%d\n",sendWindow[i].dataID);
                        
                }
            }
            int timeo = -1;
            while ( (recvack < cwnd) && (timeo = readable_timeo(sockfd, 0, 500120) > 0) ) 
            {
                Packet ack;
                Recvfrom(sockfd, (char *)&ack, sizeof(Packet), 0,(struct sockaddr *)&to, &clen);
                MaxACK = max(MaxACK, ack.dataID); // Follow receiver's feedback
                for(i = 0;i < cwnd;i++)
                    if ( (sendWindow[i].dataID+1 == ack.dataID) && (sendWindow[i].flag==0) ){
                        sendWindow[i].flag = -1;
                        //printf("Recv ACK:%d\n",sendWindow[i].dataID);
                        recvack++;
                        break;
                        }
                
            }
            if ( (timeo <= 0) && (recvack<cwnd) )
            {
                printf("Losepack.\n");
                //reTransmit(&sendWindow,cwnd);
                losepack = 1;
            }
        } 
        if (losepack==0 && cwnd+1<=MAX_WINDOW_SIZE)
            cwnd += 1;
        else if (losepack==1 && cwnd>1)
            cwnd /= 2;
    }
    printf("ReSend %d times.\n",reSendNum);
    return 0;
}
