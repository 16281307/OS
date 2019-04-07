#include<stdio.h>
#include<semaphore.h>
#include<unistd.h>
#include<fcntl.h>
#include<pthread.h>
#include<sched.h>
volatile int ticketCount=1000;
pthread_mutex_t Mylock;
void *sell(void *arg)
{
	int temp;
	pthread_mutex_lock(&Mylock);
	temp=ticketCount;
	sched_yield();
	temp=temp-1;
	sched_yield();
	ticketCount=temp;
	pthread_mutex_unlock(&Mylock);
	printf("--售票，当前剩余票数为%d--\n",ticketCount);
}
void *back(void *arg)
{
	int temp;
	pthread_mutex_lock(&Mylock);
	temp=ticketCount;
	sched_yield();
	temp=temp+1;
	sched_yield();
	ticketCount=temp;
	pthread_mutex_unlock(&Mylock);
	printf("++退票，当前剩余票数为%d++\n",ticketCount);
}
int main()
{
	pthread_mutex_init(&Mylock,NULL);
	pthread_t p1,p2;
	while(1){
	pthread_create(&p1,NULL,back,NULL);
	pthread_create(&p2,NULL,sell,NULL);
	pthread_join(p1,NULL);
	pthread_join(p2,NULL);
	}
	return 0;
}
