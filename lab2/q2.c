#include<stdio.h>
#include<unistd.h>
int main()
{
        pid_t pid;
        pid=fork();
        if(pid<0)
                printf("file\n");//创建失败
        else if(pid==0)//子进程
        {
                execl("/usr/bin/vi","vi",NULL);
        }
        else//父进程
        {
                while(1)
                {
                }
        }
        return 0;
}
