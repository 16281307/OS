#include<stdio.h>
#include<semaphore.h>
#include<unistd.h>
#include<fcntl.h>
int main()
{
	sem_t *a=sem_open("a",O_CREAT,0666,0);
	sem_t *b=sem_open("b",O_CREAT,0666,0);
	sem_t *c=sem_open("c",O_CREAT,0666,0);
	sem_t *d=sem_open("d",O_CREAT,0666,0);
	sem_t *e=sem_open("e",O_CREAT,0600,1);
	pid_t pid1;
	pid_t pid2;
	pid1=fork();
	pid2=fork();
	while(1){
	if(pid1!=0&&pid2!=0)
	{
		printf("I'm the process p1\n");
		sem_post(a);
		sem_post(b);
	}
	if(pid1!=0&&pid2==0)
	{
		sem_wait(e);
		sem_wait(a);
                printf("I'm the process p2\n");
		sem_post(c);
		sem_post(e);
	}
	if(pid1==0&&pid2!=0)
	{
		sem_wait(e);
		sem_wait(b);
		printf("I'm the process p3\n");
		sem_post(d);
		sem_post(e);
	}
	if(pid1==0&&pid2==0)
	{
		sem_wait(c);
		sem_wait(d);
		printf("I'm the process p4\n");
	}
	sleep(2);
	}
	sem_close(a);
	sem_close(b);
	sem_close(c);
	sem_close(d);
	sem_close(e);
	sem_unlink("a");
	sem_unlink("b");
	sem_unlink("c");
	sem_unlink("d");
	sem_unlink("e");
	return 0;
}
