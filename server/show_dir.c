#include "show_dir.h"
#include "../methods.h"

int Show_dir(int sockfd, struct sockaddr_in to)
{
    socklen_t clen = sizeof(to);
    DIR *dir;
    struct dirent *ptr;
    int Nid = 0; 
    printf("Show directory.\n");
    dir = opendir(DEFAULT_DIR);
    
    while( (ptr = readdir(dir) ) != NULL)
    {
        if (ptr->d_name[0]=='.')
            continue;
        Packet fn,ack;
        bzero(&fn,sizeof(Packet));
        fn.dataID = Nid++;
        strncpy(fn.data,ptr->d_name,strlen(ptr->d_name));
        fn.dataLength = strlen(fn.data);
        fn.flag = 0;
        printf("%s\n",fn.data);
        Sendto(sockfd, (char *)&fn, sizeof(Packet), 0, (struct sockaddr*)&to, clen);
        int recv = 0, timeo = 0;
        while(recv==0)
        {
            if (timeo = readable_timeo(sockfd,1,0) > 0)
            {
                Recvfrom(sockfd, (char *)&ack, sizeof(Packet), 0, (struct sockaddr*)&to, &clen);
                if (ack.dataID == fn.dataID)
                    recv = 1;
             }
             else
                Sendto(sockfd, (char *)&fn, sizeof(Packet), 0, (struct sockaddr*)&to, clen);
        }
    }
    Packet fn,ack;
    fn.dataID = Nid++;
    strncpy(fn.data,"END",strlen("END"));
    fn.dataLength = strlen(fn.data);
    fn.flag = 0;
    Sendto(sockfd, (char *)&fn, sizeof(Packet), 0, (struct sockaddr*)&to, clen);
    int recv = 0, timeo = 0;
    while(recv==0)
    {
        if (timeo = readable_timeo(sockfd,1,0) > 0)
        {
            Recvfrom(sockfd, (char *)&ack, sizeof(Packet), 0, (struct sockaddr*)&to, &clen);
            if (ack.dataID == fn.dataID)
                recv = 1;
        }
        else
            Sendto(sockfd, (char *)&fn, sizeof(Packet), 0, (struct sockaddr*)&to, clen);
    }
    closedir(dir);

}
