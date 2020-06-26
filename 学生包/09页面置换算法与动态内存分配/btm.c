#include <stdio.h>
#include <windows.h>

#define RED FOREGROUND_RED
#define GREEN FOREGROUND_GREEN
#define BLUE FOREGROUND_BLUE

#define ADDRESS		10240
#define LENGTH		102400

#define N 10  /* 定义系统允许的最大作业数 */
#define M 10  /* 定义系统允许的空闲区表最大值 */
#define MINISIZE 100	/* 若用户需要的空间比系统空闲块的容量小但是很接近时，系统将空间分配给用户后，
						系统剩余的空闲块容量将会很小，若干次分配之后，系统中就会出现一些容量
						极小总也分配不出去的空闲块，这就大大减少了分配（查找）的速度。定义此宏定
						义常量的目的就是为了弥补此缺陷：若系统分配给用户空间后剩余内存小于此值，
						则将剩余内存也分配给用户，反之，只分配给用户所需的内存*/

/* 定义表结构体 */
struct table{
	unsigned long address;	/* 内存起始地址 */
	unsigned long length;	/* 内存长度，单位为字节 */
	int flag;		/* 0表示空栏目，1表示未使用空间 */
};

struct table used_table[N] = {0};	/* 定义已分配区表 */
struct table free_table[M] = {0};	/* 定义空闲区表 */


/* 采用首次适应算法分配length大小的内存 */
void Allocate(unsigned long length)
{
	int i, k;
	unsigned long ad;
	unsigned long start;
	k = -1;
	
	/* 遍历free_table查找空间大于length的最小空闲区登记项k */
	for( i = 0; i < M; i++)
	{
		if( (free_table[i].flag == 1)&&(free_table[i].length >= length) )
		{
			if((k == -1) || free_table[i].length < free_table[k].length)
				k = i;
		}
	}
	if(k == -1){
		printf("无可用空闲区\n");
		return ;
	}
	/* 找到可用空闲区，开始分配。
	若空闲区大小与要求分配空间差小于MINISIZE大小，则空闲区全部分配；
	若空闲区大小与要求分配的空间差大于MINISIZE大小，则从空闲区划出一部分分配 */		
	if(free_table[k].length-length <= MINISIZE)
	{
		free_table[k].flag = 0;
		ad = free_table[k].address;
		length = free_table[k].length;
	}
	else
	{
		free_table[k].length = free_table[k].length - length;
		ad = free_table[k].address + free_table[k].length;
	}
	
	/* 修改已分配区表 */
	i =0;			
	while(used_table[i].flag != 0 && i<N)
		i++;
	if(i>=N) //无表目填写已分配区
	{
		printf("已分区表已满，无法继续分配内存！\n");
		if(free_table[k].flag == 0)  //修正空闲区表
			free_table[k].flag = 1;	//前面找到的是整个空闲分区
		else	//前面找到的是某个空闲分区的一部分
		{
			free_table[k].length = free_table[k].length + length;
			return ;
		}
	}
	else  /* 修改已分配区表used_table */
	{
		used_table[i].address = ad;
		used_table[i].length = length;
		used_table[i].flag = 1;
	}
	return;
}

/* 内存回收函数 */
void Reclaim(unsigned long start)
{
	int i, j, k, s, t;
	unsigned long S, L;
	s = 0;

	if(start < ADDRESS || start > (ADDRESS + LENGTH) )
	{
		printf("输入的地址有误！\n");
		return;
	}
	else{
		while(s < N)
		{
			if(start == used_table[s].address)
				break;
			s++;
		}
		if(s >= N)
		{
			printf("输入的要回收分区的首地址需要是在已分配表中存在的首地址！\n");
			return;
		}
	}

	/* 修改已分配区表used_table */
	used_table[s].flag = 0;
	// 取得归还分区的起始地址S和长度L
	S = used_table[s].address;
	L = used_table[s].length;
	used_table[s].address = 0;
	used_table[s].length = 0;
	
	j = -1;
	k = -1;
	i = 0;

	// 寻找回收分区的空闲上下邻，上邻表目k,下邻表目j
	while(i<M && (j==-1 || k==-1))
	{
		if(free_table[i].flag == 1)
		{
			if(free_table[i].address + free_table[i].length == S)/* 找到上邻 */
				k = i;
			if(free_table[i].address == S+L) /* 找到下邻 */
				j = i;
		}
		i++;
	}
	if(-1 != k)
	{
		if(-1 != j)//上邻空闲区，下邻空闲区，三区合并
		{
			free_table[k].length = free_table[j].length + free_table[k].length + L;
			free_table[j].flag = 0;
		}
		else //上邻空闲区，下邻非空闲区，与上邻合并
			free_table[k].length = free_table[k].length + L;
	}
	else if(j != -1) //上邻非空闲区，下邻空闲区，与下邻合并
	{
		free_table[j].address = S;
		free_table[j].length = free_table[j].length + L;
	}
	else // 上下邻均为非空闲区，空闲区直接填入
	{
		t = 0;
		while(free_table[t].flag == 1 && t<M)
			t++;
		if(t>=M)
		{
			printf("内存空闲表没有空间，回收空间失败\n");
			used_table[s].flag = 1;
			return;
		}
		free_table[t].address = S;
		free_table[t].length = L;
		free_table[t].flag = 1;
	}
	return;	
}

/* 设置字体颜色 */
void SetColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_INTENSITY | color);
}

int main(int argc, char* argv[])
{
        /* 注意：在应用程序中不能使用断点等调试功能 */
	int i, j, item;
	unsigned long joblength;
	unsigned long start;

	/* 空闲区表初始化 */
	free_table[0].address = ADDRESS;
	free_table[0].length = LENGTH;
	free_table[0].flag = 1;
	
	for(i=1; i<M; i++)
		free_table[i].flag = 0;
		
	for(i = 0; i<N; i++)
		used_table[i].flag = 0;

	while(1)
	{
		SetColor(RED|BLUE|GREEN);
		printf("选择功能项：(0-退出  1-分配内存  2-回收内存  3-显示内存)：");
		scanf("%d",&item);
		switch(item)
		{
			case 0:
				exit(0);
			case 1:
				SetColor(RED|GREEN);
				printf("所需内存长度：");
				scanf("%*c%ld",&joblength);
				Allocate(joblength);
				break;
			case 2:
				SetColor(RED|BLUE);
				printf("输入要回收分区的首地址：");
				scanf("%*c%ld",&start);
				Reclaim(start);
				break;
			case 3:
				SetColor(RED|GREEN|BLUE);
				printf("空闲区表：\n起始地址 分区长度 标志\n");
				SetColor(GREEN);
				for(i=0; i<M; i++)
					printf("%6ld%9ld%6d\n",free_table[i].address,free_table[i].length,free_table[i].flag);
				
				SetColor(RED|GREEN|BLUE);
				printf("输入任意字符输出已分配表...\n");
				getch();
				printf("已分配区表：\n起始地址 分区长度\n");	
				SetColor(RED);
				for(i=0; i<N; i++)
				{
					if(0 != used_table[i].flag)
						printf("%6ld%9ld\n",used_table[i].address,used_table[i].length);
					else
						printf("%6ld%9ld\n",used_table[i].address,used_table[i].length);
				}
				break;
			default:
				printf("没有该选项\n");
		}
	}
}
