#include<stdio.h>
#include<unistd.h>
int main()
{
        pid_t p1;
        pid_t p2;
        pid_t p3;
        p1=fork();
        p2=fork();
        if(p1==0&&p2>0)
        {
                p3=fork();
        }
        if(p1==0&&p2>0&&p3>0)
        {
                *(char *)0=0;
        }

        while(1)
        {
                printf("parend pid is %d and child pid is %d\n",getppid(),getpid());
                sleep(3);
        }
        return 0;
}
