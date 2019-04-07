/*
 * Filename: pipe.c
 */
 
#include <stdio.h>
#include <unistd.h>     //for pipe()
#include <string.h>     //for memset()
#include <stdlib.h>     //for exit()

int main()
{
    int fd[2];
    char buf[20];
    if(-1 == pipe(fd))
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    int choice;
    int flag;
    printf("请输入选项:\n1.写入\n2.读取\n0.退出\n");
    while(1)
    {
	    scanf("%d",&choice);
	    if(choice==1)
	    {
    		flag=write(fd[1], "hello,world", 12);
		memset(buf, '\0', sizeof(buf));
	    	if(flag!=-1)
			printf("写入成功\n");
		else
			printf("filed\n");
	    }
	    if(choice==2)
	    {
    		flag=read(fd[0], buf, 12);
		printf("The message is: %s\n", buf);
	    	if(flag!=-1)
			printf("读取成功\n");
		else
			printf("filed\n");
	    }
	    if(choice==0)
		    return 0;
    }
}
