#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>

#define BUFFER_SIZE 512 
#define FILE_NAME_MAX_SIZE 512
#define DEFAULT_DIR "/home/fs302/file_server/server"

int copyfile(char *source, char *target);
int check_md5(char *message);
