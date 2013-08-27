#include "check_md5.h"
#include "../md5.h"

// 服务器本地复制 source->target
int copyfile(char *source, char *target)
{
    FILE *fps,*fpt;
    if ( (fps = fopen(source,"r")) == NULL)
    {
        printf("open %s failed.\n",source);
        return -1;
    }
    if ( (fpt = fopen(target,"w")) == NULL)
    {
        printf("open %s failed.\n",target);
        return -1;
    }
    int FileNotEnd = 1;
    int file_block_length = 0,file_write_length = 0;
    char buffer[BUFFER_SIZE];
    bzero(&buffer,BUFFER_SIZE);
    while( (file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fps)) > 0)
    {
        
        file_write_length = fwrite(buffer, sizeof(char), file_block_length, fpt);
        if (file_write_length != file_block_length)
        {
            printf("Write failed.\n");
            return -1;
        }
        bzero(&buffer,BUFFER_SIZE);
    }
    fclose(fps);
    fclose(fpt);
    return 0;
}

int check_md5(char *message)
{
    // Step1: 获取文件名列表 
    char dir_files[BUFFER_SIZE][BUFFER_SIZE]; // filenames in directory
    int record_exist[BUFFER_SIZE];            // flag of file record in db
    DIR *dir;
    struct dirent *ptr;
    int i = 0,file_num = 0;
    char cur_dir[BUFFER_SIZE];
    getcwd(cur_dir, BUFFER_SIZE);
    dir = opendir(cur_dir);
    while( NULL != (ptr = readdir(dir)))
    {
        if (ptr->d_name[0]=='.')
            continue;
        strncpy(dir_files[file_num],ptr->d_name,strlen(ptr->d_name));
        record_exist[file_num] = 0;
        file_num++;
    }
    // Step2: 检查数据库 
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
    // Step3: 更新数据库，比对message 
    db = fopen("md5_db.txt","w");
    int ret = 0;
    char message_file[BUFFER_SIZE],message_md5[33]={'\0'};
    sscanf(message,"%s\t%d\t%s",message_file,&filesize,message_md5);
    for(i = 0;i < Nid;i++)
    {
        fputs(db_list[i],db);
        sscanf(db_list[i],"%s\t%d\t%s",filename,&filesize,md5sum);
        if (strncmp(md5sum,message_md5,strlen(message_md5))==0)
        {
            // 再次确认实际文件的md5,因为有可能文件被窜改
            FILE *fp;
            char md5_recheck[33]={'\0'};
            fp = fopen(filename,"rb");
            md5(&filesize,md5_recheck,&fp);
            if (strncmp(md5_recheck,message_md5,strlen(message_md5))==0)
            {
                ret = 1; // 秒传结论来之不易呀！
                if(strncmp(db_list[i],message,strlen(message)) != 0)
                    copyfile(filename,message_file);
            }
        }
    }
    if (ret==0)
    {
        sscanf(message,"%s\t%d\t%s",message_file,&filesize,message_md5);
        sprintf(buffer,"%s\t%d\t%s\n",message_file,filesize,message_md5);
        fputs(buffer,db);
    }
    fclose(db);
    printf("%s\n",message);
    return ret;
}
