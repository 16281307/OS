#include<stdio.h>
#include<unistd.h>
int main()
{
        pid_t p1;
        pid_t p2;
        p1=fork();
        p2=fork();
        if(p1==0&&p2>0)
        {
                fork();
        }
        printf("parend pid is %d and child pid is %d\n",getppid(),getpid());
        while(1);
        return 0;
}
