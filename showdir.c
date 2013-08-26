#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    DIR *dir;
    struct dirent *ptr;

    dir = opendir("/home/fs302/file_server");

    while( (ptr = readdir(dir)) != NULL)
    {
        if (ptr->d_name[0]=='.')
            continue;
        printf("%s\n",ptr->d_name);
    }

    closedir(dir);
    return 0;
}
