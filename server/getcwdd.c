#include <stdio.h>
#include <dirent.h>
int main()
{
    char buffer[512];
    getcwd(buffer, 512);
    printf("The current directory is:%s\n",buffer);
    return 0;
}
