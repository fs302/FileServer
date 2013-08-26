#include "md5.h"

int md5(int *filesize, char *md5sum,FILE **fpp)
{
    MD5_CTX ctx;
    unsigned char data[DATA_SIZE];
    unsigned char md[16];
    char buf[33]={'\0'};
    char tmp[3] = {'\0'};
    int i,len;
    FILE *fp;
    fp = *fpp;
    fseek(fp, 0, SEEK_END);
    if ((len=ftell(fp))==-1){
        printf("Sorry! Cannot calculate files which is larger then 2GB!\n");
        fclose(fp);
        return 0;
    }
    *filesize = len;
    rewind(fp);
    MD5_Init(&ctx);
 
    int FileNotEnd = 1, file_block_length = 0;
    while(FileNotEnd)
    {
       file_block_length = fread(data, sizeof(char), DATA_SIZE, fp); 
       if(file_block_length <= 0){
           FileNotEnd = 0;
           break;
       }
       MD5_Update(&ctx, data, file_block_length);
    }
    MD5_Final(md, &ctx);

    for(i=0;i<16;i++)
    {
        sprintf(tmp,"%02x",md[i]);
        strcat(buf,tmp);
    }
    sprintf(md5sum,"%s",buf);
    rewind(fp);
    return 0;
}
