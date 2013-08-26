#include <netinet/in.h>  // for sockaddr_in
#include <sys/types.h>   // for socket
#include <sys/socket.h>  // for socket
#include <sys/select.h>  // for select
#include <unistd.h>      // for alarm      
#include <stdlib.h>      // for exit,rand
#include <string.h>      // for bzero
#include <stdio.h>       // for printf
#include <time.h>        // for clock

int Sendto(int sockfd,const void *buff, size_t nbytes, int flags, const struct sockaddr *to, socklen_t addrlen);

int Recvfrom(int sockfd,void *buff, size_t nbytes, int flags, struct sockaddr *from, socklen_t *addrlen);

int readable_timeo(int fd, int sec, int usec);
