#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<fcntl.h>
#include<unistd.h>
char buffer[10];
sem_t *buf=NULL;
sem_t *date=NULL;
int i=0,j=0;
void *readin(void *arg)
{
	
	while(1)
	{
		sem_wait(buf);
		scanf("%c",&buffer[i%10]);
		i=i+1;
		sem_post(date);
	}
}
void *print(void *arg)
{
	
	while(1)
	{
		sem_wait(date);
		printf("%c",buffer[j%10]);
		j=j+1;
		sem_post(buf);
		sleep(1);
	}
}
int main()
{
	buf=sem_open("mysem",O_CREAT,0666,10);//可用空间数目
	date=sem_open("mysem2",O_CREAT,0666,0);//空间内已有数据数目
	pthread_t p1,p2;
	pthread_create(&p1,NULL,readin,NULL);
	pthread_create(&p2,NULL,print,NULL);
	pthread_join(p1,NULL);
	pthread_join(p2,NULL);
	sem_close(buf);
	sem_close(date);
	sem_unlink("mysem");
	sem_unlink("mysem2");
	return 0;
}
