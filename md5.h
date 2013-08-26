#include <stdio.h>
#include <openssl/md5.h>
#include <string.h>

#define DATA_SIZE 64

int md5(int *filesize, char *md5sum, FILE **fpp);
