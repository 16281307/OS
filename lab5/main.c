#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define L 500//逻辑块的总数目
#define B 10//每块的大小
#define K 100  //文件系统的保留区
#define BUSY 1
#define FREE 0
#define OK 1
#define ERROR 0
#define FILE_BLOCK_LENGTH (B-3)
#define FILE_NAME_LENGTH (B-1)
#define FILE_SIGN_AREA ((L-1-K)/B+1)
#define FILE_NUM FILE_BLOCK_LENGTH
#define BUFFER_LENGTH 25
#define INPUT_LENGTH 100
#define OUTPUY_LENGTH 100
struct filesign {
	int file_length;
	int filesign_flags;
	int file_block;
	int file_block_ary[FILE_BLOCK_LENGTH];
};
struct contents{
	char filename[FILE_NAME_LENGTH];
	int filesignum;
};
struct openfilelist {
	char buffer[BUFFER_LENGTH];
	int pointer[2];
	int filesigunm;
	int flag;
};
char ldisk[L][B];
openfilelist open_list[FILE_NUM];

void read_block(int i, char *p)//把逻辑块i的内容读入到指针P指向的内存位置，每次读入B块
{
	char *temp = (char *)malloc(sizeof(char));
	temp = p;
	for (int j = 0; j < B;j++)
	{
		*temp = ldisk[i][j];
		temp++;
	}
}

void write_block(int i, char *p)//把指针P的内容写入逻辑块i，每次写入B块
{
	char *temp = (char*)malloc(sizeof(char));
	temp = p;
	for (int j = 0; j < B; j++)
	{
		ldisk[i][j] =*temp;
		temp++;
	}
}

void init_block(char *temp, int length)//初始化一个字符数组块
{
	for (int i = 0; i < length; i++)
	{
		temp[i] = '\0';
	}
}

int write_buffer(int index, int list)
{
	int i, j, freed;
	char temp[B];
	int buffer_length = BUFFER_LENGTH;
	for (i = 0; i < BUFFER_LENGTH; i++)
	{
		if (open_list[list].buffer[i] == '\0')
		{
			buffer_length = i;
			break;
		}
	}
	int x = open_list[list].pointer[0];
	int y = open_list[list].pointer[1];
	int z = B - y;
	if (buffer_length < z)
	{
		read_block(x, temp);
		strncat(temp + y, open_list[list].buffer,buffer_length);
		write_block(x, temp);
		read_block(index + FILE_SIGN_AREA, temp);
		temp[1] += buffer_length;
		write_block(index + FILE_SIGN_AREA, temp);
		open_list[list].pointer[0] = x;
		open_list[list].pointer[1] = y + buffer_length;
	}
	else
	{
		read_block(index + FILE_SIGN_AREA, temp);
		if (temp[2] + (buffer_length - z) / B + 1 > FILE_BLOCK_LENGTH)
		{
			printf("文件分配空间不足\n");
			return ERROR;
		}
		read_block(x, temp);
		strncat(temp + y, open_list[list].buffer, z);
		write_block(x, temp);
		for (i = 0; i < (buffer_length - z) / B; i++)
		{
			for (j = K + FILE_NUM; j < L; j++)
			{
				read_block((j - K) / B, temp);
				if (temp[(j - K) % B] == FREE)
				{
					freed = j;
					break;
				}
			}
			if (j == L)
			{
				printf("磁盘已满，分配失败\n");
				return ERROR;
			}
			init_block(temp, B);
			strncpy(temp, (open_list[list].buffer + z + (i*B)), B);
			write_block(freed, temp);
			read_block((freed - K) / B, temp);
			temp[(freed-K)%B]=BUSY;
			write_block((freed-K)/B,temp);
			read_block(index + FILE_SIGN_AREA, temp);
			temp[2]++;
			temp[2 + temp[2]] = freed;
			write_block(index + FILE_SIGN_AREA, temp);
		}
		for (j = K + FILE_NUM; j < L; j++)
		{
			read_block((j - K) / B, temp);
			if (temp[(j - K) % B] == FREE)
			{
				freed = j;
				break;
			}
			if (j == L)
			{
				printf("磁盘已满，分配失败\n");
				return ERROR;
			}
			init_block(temp, B);
			strncpy(temp, (open_list[list].buffer + z + (i*B)), (buffer_length-z)%B);
			write_block(freed, temp);
			read_block((freed - K) / B, temp);
			temp[(freed - K) % B] = BUSY;
			write_block((freed - K) / B, temp);
			read_block(index + FILE_SIGN_AREA, temp);
			temp[2]++;
			temp[2 + temp[2]] = freed;
			write_block(index + FILE_SIGN_AREA, temp);
			read_block(index+FILE_SIGN_AREA, temp);
			temp[1] += buffer_length;
			write_block(index + FILE_SIGN_AREA, temp);
			open_list[list].pointer[0] = freed;
			open_list[list].pointer[1] = (buffer_length - z) % B;
		}
	}
}

int lseek(int index, int pos)
{
	int i;
	int list = -1;
	char temp[B];
	int pos_i = pos / B;
	int pos_j = pos % B;
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesigunm == index)
		{
			list = i;
			break;
		}
	}
	if (list == -1)
	{
		printf("没找到当前索引号文件，操作失败\n");
		return ERROR;
	}
	if (open_list[list].flag != BUSY)
	{
		printf("输入索引号有误，操作失败\n");
		return ERROR;
	}
	read_block(open_list[list].filesigunm + FILE_SIGN_AREA, temp);
	if (pos_i > temp[2] - 1)
	{
		printf("异常越界，定位失败");
		return ERROR;
	}
	open_list[list].pointer[0] = temp[3 + pos_i];
	open_list[list].pointer[1] = pos_j;
	return OK;
}

void init()
{
	int i;
	char temp[B];
	for (i = 0; i < L; i++)
	{
		init_block(temp,B);
		write_block(i, temp);
	}
	for (i = K; i < L; i++)
	{
		read_block((i - K) / B, temp);
		temp[(i - K) % B] = FREE;
		write_block((i - K) % B, temp);
	}
	filesign temp_cnt_sign;
	temp_cnt_sign.filesign_flags = 1;
	temp_cnt_sign.file_length = 0;
	temp_cnt_sign.file_block = FILE_BLOCK_LENGTH;
	init_block(temp, B);
	temp[0] = temp_cnt_sign.filesign_flags;
	temp[1] = temp_cnt_sign.file_length;
	temp[2] = temp_cnt_sign.file_block;
	for (i = 0; i < FILE_BLOCK_LENGTH; i++)
	{
		temp[i + 3] = K + i;
	}
	write_block(FILE_SIGN_AREA, temp);
	read_block(0, temp);
	for (i = 0; i < FILE_NUM; i++)
	{
		temp[i] = FREE;
	}
	write_block(0, temp);
}

int create(char filename[])//创建文件
{
	int i, frees, freed, freed2;
	char temps[B], tempc[B], temp[B];
	for (i = K; i < K + FILE_NUM; i++)
	{
		read_block((i - K) / B, temp);
		if (temp[(i - K) % B] == BUSY)
		{
			read_block(i, temp);
			if (strncmp(temp + 1, filename, FILE_NAME_LENGTH) == 0)
			{
				printf("该目录已存在文件名为%s的文件\n", filename);
				return ERROR;
			}
		}
	}
	for (i = FILE_SIGN_AREA; i < K; i++)
	{
		read_block(i, temp);
		if (temp[0] == FREE)
		{
			frees = i;
			break;
		}
	}
	if (i == K)
	{
		printf("没有空闲的文件描述符\n");
		return ERROR;
	}
	for (i = K; i < K + FILE_NUM; i++)
	{
		read_block((i - K) / B, temp);
		if (temp[(i - K) % B] == FREE)
		{
			freed = i;
			break;
		}
	}
	if (i == K + FILE_NUM)
	{
		printf("文件数已达上限\n");
		return ERROR;
	}
	for (i = K + FILE_NUM; i < L; i++)
	{
		read_block((i - K) / B, temp);
		if (temp[(i - K) % B] == FREE)
		{
			freed2 = i;
			break;
		}
	}
	if (i == L)
	{
		printf("磁盘已满，分配失败\n");
		return ERROR;
	}
	filesign temp_filesign;
	contents temp_contents;
	temp_filesign.filesign_flags = 1;
	temp_filesign.file_length = 0;
	temp_filesign.file_block = 1;
	init_block(temps, B);
	temps[0] = temp_filesign.filesign_flags;
	temps[1] = temp_filesign.file_length;
	temps[2] = temp_filesign.file_block;
	temps[3] = freed2;
	for (i = 4; i < FILE_BLOCK_LENGTH; i++)
	{
		temps[i] = '\0';
	}
	write_block(frees, temps);
	temp_contents.filesignum = frees - FILE_SIGN_AREA;
	strncpy(temp_contents.filename, filename, FILE_NAME_LENGTH);
	init_block(tempc, B);
	tempc[0] = temp_contents.filesignum;
	tempc[1] = '\0';
	strcat(tempc, temp_contents.filename);
	write_block(freed, tempc);
	read_block((freed - K) / B, temp);
	temp[(freed - K) % B] = BUSY;
	write_block((freed - K) / B, temp);
	read_block((freed2 - K) / B, temp);
	temp[(freed2 - K) % B] = BUSY;
	write_block((freed2 - K) / B, temp);
	read_block(FILE_SIGN_AREA, temp);
	temp[1]++;
	write_block(FILE_SIGN_AREA, temp);
	return OK;
}

int destroy(char *filename)
{
	int i, dtys, dtyd, use_block, index;
	char temp[B];
	char tempd[B];
	for (i = K; i < K + FILE_NUM; i++)
	{
		read_block((i - K) / B, temp);
		if (temp[(i - K) % B] == BUSY)
		{
			read_block(i, temp);
			if (strncmp(temp + 1, filename, FILE_NAME_LENGTH) == 0)
			{
				dtyd = i;
				dtys = temp[0] + FILE_SIGN_AREA;
				index = temp[0];
				break;
			}
		}
	}
	if (i == K + FILE_NUM)
	{
		printf("没有找到该文件\n");
		return ERROR;
	}
	int list = -1;
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesigunm == index)
		{
			list = i;
			break;
		}
	}
	if (open_list[list].flag == BUSY && list != -1)
	{
		printf("该文件已经被打开，需要关闭才能删除\n");
		return ERROR;
	}
	read_block(dtys, temp);
	use_block = temp[2];
	for (i = 0; i < use_block; i++)
	{
		read_block((temp[i + 3] - K) / B, tempd);
		tempd[(temp[i + 3] - K) % B] = FREE;
		write_block((temp[i + 3] - K) / B, tempd);
	}
	init_block(temp, B);
	write_block(dtys, temp);
	init_block(temp, B);
	write_block(dtyd, temp);
	read_block((dtyd - K) / B, temp);
	temp[(dtyd - K) % B] = FREE;
	write_block((dtyd - K) / B, temp);
	read_block(FILE_SIGN_AREA, temp);
	temp[1]--;
	write_block(FILE_SIGN_AREA,temp);
	return OK;
}

int open(char *filename)
{
	int i, opd, ops, list, index;
	char temp[B];
	for (i = K; i < K + FILE_NUM; i++)
	{
		read_block((i - K) / B, temp);
		if (temp[(i - K) % B] == BUSY)
		{
			read_block(i, temp);
			if (strncmp(temp + 1, filename, FILE_NAME_LENGTH) == 0)
			{
				opd = i;
				ops = temp[0];
				break;
			}
		}
	}
	if (i == K + FILE_NUM)
	{
		printf("没有找到该文件\n");
		return ERROR;
	}
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesigunm == ops && open_list[i].flag == BUSY)
		{
			printf("该文件已被打开\n");
			return ERROR;
		}
	}
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].flag != BUSY)
		{
			list = i;
			break;
		}
	}
	open_list[list].filesigunm = ops;
	open_list[list].flag = BUSY;
	index = open_list[list].filesigunm;
	lseek(index, 0);
	init_block(open_list[list].buffer, BUFFER_LENGTH);
	read_block(open_list[list].pointer[0], temp);
	strncpy(open_list[list].buffer, temp, BUFFER_LENGTH);
	return OK;
}

int close(int index)
{
	int i;
	int list = -1;
	char temp[B];
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesigunm == index)
		{
			list = i;
			break;
		}
	}
	if (list == -1)
	{
		printf("没找到当前做引号文件，操作失败\n");
		return ERROR;
	}
	if (open_list[list].flag != BUSY)
	{
		printf("输入索引号有误，操作失败\n");
		return ERROR;
	}
	write_buffer(index, list);
	init_block(open_list[list].buffer, BUFFER_LENGTH);
	open_list[list].filesigunm = FREE;
	open_list[list].flag = FREE;
	open_list[list].pointer[0] = NULL;
	open_list[list].pointer[1] = NULL;
	return OK;
}

int read(int index, int mem_area, int count)
{
	int i;
	int list = -1;
	char temp[B];
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesigunm == index)
		{
			list = i;
			break;
		}
	}
	if (list == -1)
	{
		printf("没找到当前索引号文件，操作失败\n");
		return ERROR;
	}
	if (open_list[list].flag != BUSY)
	{
		printf("输入的索引号有误，操作失败\n");
		return ERROR;
	}
	char temp_output[OUTPUY_LENGTH];
	init_block(temp_output, OUTPUY_LENGTH);
	char output[OUTPUY_LENGTH];
	init_block(output, OUTPUY_LENGTH);
	read_block(FILE_SIGN_AREA + index, temp);
	int file_length = temp[1];
	int file_block = temp[2];
	int file_area;
	for (i = 0; i < file_block - 1; i++)
	{
		read_block(FILE_SIGN_AREA + index, temp);
		read_block(temp[3 + i], temp);
		strncpy(temp_output + i * B, temp, B);
	}
	read_block(FILE_SIGN_AREA+index, temp);
	read_block(temp[3 + i], temp);
	strncpy(temp_output + i * B, temp, B);
	int x = open_list[list].pointer[0];
	int y = open_list[list].pointer[1];
	for (i = 0; i < file_block; i++)
	{
		read_block(FILE_SIGN_AREA + index, temp);
		if (temp[3 + i] == x)
		{
			break;
		}
	}
	file_area = i * B + y;
	for (i = 0; i < count; i++)
	{
		output[i + mem_area] = temp_output[i + file_area];
	}
	printf("%s\n", output + mem_area);
	return OK;
}

int write(int index, int mem_area, int count)
{
	int i;
	int list = -1;
	int input_length;
	char temp[B];
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesigunm == index)
		{
			list = i;
			break;
		}
	}
	if (list == -1)
	{
		printf("没找到当前索引号文件，操作失败\n");
		return ERROR;
	}
	if (open_list[list].flag != BUSY)
	{
		printf("输入索引号错误，操作失败\n");
		return ERROR;
	}
	char input[INPUT_LENGTH];
	init_block(input, INPUT_LENGTH);
	fflush(stdin);
	for(i=0;;i++)
	{
		scanf("%c", &input[i]);
		if (input[i] == '\n')
		{
			input[i] = '\0';
			break;
		}
	}
	while (scanf("%c", &input[i]))
	{
		if (input[i] == '\n')
		{
			input[i] = '\0';
			break;
		}
		i++;
	}
	input_length = i;
	if (count <= BUFFER_LENGTH)
	{
		strncat(open_list[list].buffer, input + mem_area, count);
	}
	else
	{
		int rest;
		for (i = 0; i < BUFFER_LENGTH; i++)
		{
			if (open_list[list].buffer[i] == FREE)
			{
				rest = BUFFER_LENGTH - i;
				break;
			}
		}
		strncat(open_list[list].buffer + BUFFER_LENGTH - rest, input + mem_area, rest);
		write_buffer(index, list);
		init_block(open_list[list].buffer, BUFFER_LENGTH);
		for (i = 0; i < (count / BUFFER_LENGTH) - 1; i++)
		{
			strncpy(open_list[list].buffer, (input + mem_area) + rest + i * BUFFER_LENGTH, BUFFER_LENGTH);
			write_buffer(index, list);
			init_block(open_list[list].buffer, BUFFER_LENGTH);
		}
		init_block(open_list[list].buffer, BUFFER_LENGTH);
		strncpy(open_list[list].buffer, (input + mem_area) + rest + i*BUFFER_LENGTH, count%BUFFER_LENGTH);
		int buffer_start;
	}
	return OK;
}

void directory()
{
	int i, filenum, filelength;
	char filename[FILE_NAME_LENGTH], temp[B], tempd[B], temps[B];
	read_block(FILE_SIGN_AREA, temp);
	filenum = temp[1];
	printf("\n");
	if (filenum == 0)
	{
		printf("该目录下没有文件\n");
	}
	for (i = 0; i < FILE_NUM; i++)
	{
		read_block(temp[3 + i], tempd);
		if (tempd[0] != 0)
		{
			read_block(tempd[0] + FILE_SIGN_AREA, temps);
			if (temps[0] == BUSY && tempd[0] != 0)
			{
				filelength = temps[1];
				strcpy(filename, tempd + 1);
				printf("%s\t\t%d字节\n", filename, filelength);
			}
		}
	}
	if (filenum != 0)
	{
		printf("\t\t\t\t共%d个文件\n", filenum);
	}
}

int show_openlist()
{
	int i, j, openfile = 0, index;
	char temp[B];
	printf("\n索引号\t\t大小\t\t文件名\n");
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].flag == BUSY)
		{
			index = open_list[i].filesigunm;
			printf("%d", index);
			openfile++;
			read_block(FILE_SIGN_AREA + index, temp);
			printf("\t\t%d", temp[1]);
			for (j = K ;j < K + FILE_NUM; j++)
			{
				read_block(j, temp);
				if (temp[0] == index)
				{
					printf("\t\t%s\n", temp + 1);
				}
			}
		}
	}
	return openfile;
}

void show_ldisk()
{
	int a, b;
	for (a = 0; a < K + 30; a++)
	{
		printf("%d", a);
		for (b = 0; b < B; b++)
		{
			printf("%d", ldisk[a][b]);
		}
		printf("\n");
	}
}

void menu()
{
	printf("1.查看目录\n2.创建文件\n3.删除文件\n4.打开文件\n5.关闭文件\n6.读取文件内容\n7.修改文件内容\n8.查看文件列表\n9.展示磁盘信息\n");
}

void main()
{
	init();
	char filename[FILE_NAME_LENGTH];
	init_block(filename, FILE_NAME_LENGTH);
	menu();
	int choice;
	while(true)
	{
	//	menu();
		printf("请输入命令\n");
		scanf("%d", &choice);
		if (choice == 1)//查看目录
		{
			directory();
			printf("-------------------------------------------\n");
		}
		if (choice == 2)//创建文件
		{
			printf("请输入文件名\n");
			scanf("%s", &filename);
			if (create(filename) == OK)
				printf("文件%s创建成功\n", filename);
			else
				printf("文件%s创建失败\n", filename);
			printf("-------------------------------------------\n");
		}
		if (choice == 3)//删除文件
		{
			printf("请输入文件名\n");
			scanf("%s", &filename);
			if (destroy(filename) == OK)
				printf("文件%s删除成功\n", filename);
			else
				printf("文件%s删除失败\n", filename);
			printf("-------------------------------------------\n");
		}
		if (choice == 4)//打开文件
		{
			printf("请输入文件名\n");
			scanf("%s", &filename);
			if (open(filename) == OK)
				printf("文件%s打开成功\n", filename);
			else
				printf("文件%s打开失败\n", filename);
			printf("-------------------------------------------\n");
		}
		if (choice == 5)//关闭文件
		{
			int index;
			if (show_openlist()== 0)
			{
				printf("当前没有文件被打开\n");
			}
			else
			{
				printf("请输入要关闭文件的索引号\n");
				scanf("%d", &index);
				if (close(index) == OK)
					printf("文件关闭成功\n");
			}
			printf("-------------------------------------------\n");
		}
		if (choice == 6)//读取文件内容
		{
			int index, count;
			if (show_openlist() == 0)
			{
				printf("当前没有文件被打开\n");
			}
			else
			{
				printf("请输入要打开文件的索引号\n");
				scanf("%d", &index);
				printf("请输入要读取的文件长度\n");
				scanf("%d", &count);
				if (read(index, 0, count) == OK)
					printf("读取文件操作成功\n");
			}
			printf("-------------------------------------------\n");
		}
		if (choice == 7)//修改文件内容
		{
			int index, count;
			if (show_openlist() == 0)
			{
				printf("当前没有文件被打开\n");
			}
			else
			{
				printf("请输入要写入文件的索引号\n");
				scanf("%d", &index);
				printf("请输入要写入的文件长度\n");
				scanf("%d", &count);
				if (write(index, 0, count) == OK)
					printf("写入文件操作成功\n");
			}
			printf("-------------------------------------------\n");
		}
		if (choice == 8)//查看文件列表
		{
			if (show_openlist() == 0)
			{
				printf("当前没有文件被打开\n");
			}
			printf("-------------------------------------------\n");
		}
		if (choice == 9)//展示磁盘信息
		{
			show_ldisk;
			printf("-------------------------------------------\n");
		}
		if (choice == 0)
		{
			return;
		}
	}
}
