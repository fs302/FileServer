#include "check_md5.h"
#include "../md5.h"

int check_md5(char *message)
{
    // Step1: get directory list
    char dir_files[BUFFER_SIZE][BUFFER_SIZE]; // filenames in directory
    int record_exist[BUFFER_SIZE];            // flag of file record in db
    DIR *dir;
    struct dirent *ptr;
    int i = 0,file_num = 0;
    dir = opendir(DEFAULT_DIR);
    while( NULL != (ptr = readdir(dir)))
    {
        if (ptr->d_name[0]=='.')
            continue;
        strncpy(dir_files[file_num],ptr->d_name,strlen(ptr->d_name));
        record_exist[file_num] = 0;
        file_num++;
    }
    // Step2: get md5 database list & check record_exist
    FILE *db;
    char buffer[BUFFER_SIZE];
    char filename[BUFFER_SIZE],md5sum[33]={'\0'};
    char db_list[BUFFER_SIZE][BUFFER_SIZE];
    int filesize = 0, Nid = 0;
    db = fopen("md5_db.txt","r");
    while( fgets(buffer,BUFFER_SIZE, db) != NULL)
    {
        bzero(&filename,sizeof(filename));
        bzero(&md5sum,sizeof(md5sum));
        sscanf(buffer, "%s\t%d\t%s", filename, &filesize,md5sum); 
        int file_exist = 0;
        for(i = 0; i < file_num;i++)
        {
            if (strncmp(dir_files[i],filename,strlen(filename))==0)
            {
                file_exist = 1;
                record_exist[i] = 1;
                break;
            }
        }
        // Check if record in db still exist in dir
        if (file_exist==1)
        {
            strncpy(db_list[Nid++],buffer,strlen(buffer));
        }
    }
    fclose(db);
    //Add new file to Database
    for(i = 0; i < file_num;i++)
    {
        if(!record_exist[i])
        {
            FILE *fp;
            fp = fopen(dir_files[i], "rb");
            md5(&filesize, md5sum,&fp);
            sprintf(buffer,"%s\t%d\t%s\n",dir_files[i],filesize,md5sum);
            strncpy(db_list[Nid++],buffer,strlen(buffer));
            fclose(fp);
        }
    }
    // Step3: write database & Check message
    db = fopen("md5_db.txt","w");
    int ret = 0;
    for(i = 0;i < Nid;i++)
    {
        fputs(db_list[i],db);
        if (strncmp(db_list[i],message,strlen(message))==0)
            ret = 1;
    }
    fclose(db);
    printf("%s\n",message);
    printf("checkmd5 finishd with %d\n",ret); 
    return ret;
}
