#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>

#define BUFFER_SIZE 512 
#define FILE_NAME_MAX_SIZE 512

int copyfile(char *source, char *target);
int check_md5(char *message);
