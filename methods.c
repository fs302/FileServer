#include "methods.h"

int Sendto(int sockfd,const void *buff, size_t nbytes, int flags, const struct sockaddr *to, socklen_t addrlen)
{
    int nSendBytes = sendto(sockfd, buff, nbytes, flags, to, addrlen);
    if ( nSendBytes <= 0)
    {
        printf("Send error with result: %d\n", nSendBytes);
        return -1;
    }
    return 0;
}

int Recvfrom(int sockfd,void *buff, size_t nbytes, int flags, struct sockaddr *from, socklen_t *addrlen)
{
    int ret = recvfrom(sockfd, buff, nbytes, flags, from, addrlen);
    if (ret < 0)
    {
        printf("Recv from Server error\n");
        return -1;
    }
    return 0;
}

int readable_timeo(int fd, int sec, int usec)
{
    fd_set rset;
    struct timeval tv;

    FD_ZERO(&rset);
    FD_SET(fd, &rset);

    tv.tv_sec = sec;
    tv.tv_usec = usec;

    return(select(fd+1, &rset, NULL, NULL, &tv));
        /* >0 if descriptor is readable*/
}
