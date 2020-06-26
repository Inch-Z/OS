#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>

#define BLOCKSIZE 5			//内存块大小
#define PAGENUMOFRSIZE 15	//页面引用串大小

//输出内存块页面序号
int OutBlockofM(int *BlockofMemory, int BlockSize, int flag)
{
	int i;
	printf("内存中页块为：\t");
	for(i = 0; i < BlockSize; i++)
		printf("%d  ",BlockofMemory[i]);
	printf("\t");
	if(flag != -1)
		printf("淘汰页面号为：%d",flag);
	printf("\n");
	return -1;
}

//输出页面引用串号
void OutPageNumofR(int* PageNumofR, int PageNumRefSize)
{
	int i = 0;
	printf("页面引用串是：\t");
	for(i = 0; i < PageNumRefSize; i++)
		printf("%d  ",PageNumofR[i]);
	printf("\n");
}

//内存块页面序号清零
void QuitBofM(int *BlockofMemory, int BlockSize)
{
	int i;
	for(i = 0; i < BlockSize; i++)
		BlockofMemory[i] = 0;
}

//判断页是否在内存中，如果页在内存中，返回1，否则返回0；
int PageInBofM(int PageNum,int *BlockofMemory,int BlockSize)
{
	int i;
	for(i = 0; i < BlockSize; i++)
		if(PageNum == BlockofMemory[i])
			return 1;
	return 0;
}

//下次访问次序
//参数j:  页面在内存块中的位置
//参数i： 页面号在页面号引用串中的位置
int DistanceOtp(int *BlockofMemory, int *PageNumofR, int j, int i, int PageNumRefSize)
{
	int k;
	for(k = i + 1; k < PageNumRefSize; k++)
		if(BlockofMemory[j] == PageNumofR[k])
			return k;
	return PageNumRefSize;
}

//最近一次访问次序
//参数j:  页面在内存块中的位置
//参数i： 页面号在页面号引用串中的位置
int DistanceLru(int *BlockofMemory,int *PageNumofR,int j,int i)
{
	int k;
	int num = 0;
	for(k = i-1; k > 0; k--)
		if(BlockofMemory[j] == PageNumofR[k])
		{
			num = i-k;
			return num;
		}
	return i;
}

//最佳页面置换算法
void Opt(int *BlockofMemory,int *PageNumofR,int BlockSize,int PageNumRefSize,int flag,int count)
{
	int i, j, k = 0;
	int buffer1 = 0, buffer2 = 0;
	printf("**********最佳页面置换算法：**********\n");
	//输出页面引用串号
	OutPageNumofR(PageNumofR, PageNumRefSize);
	
	//输出内存块页面序号
	for(i = 0; i < BlockSize; i++)
	{
		BlockofMemory[i] = PageNumofR[i];
		flag = OutBlockofM(BlockofMemory,BlockSize,flag);
	}
	for(i = BlockSize;i < PageNumRefSize; i++)
	{
		if(!PageInBofM(PageNumofR[i], BlockofMemory, BlockSize))//页不在内存中
		{
			//求出最长未来时间不被访问的页
			for(j = 0; j < BlockSize; j++)
			{
				buffer2=DistanceOtp(BlockofMemory,PageNumofR,j,i,PageNumRefSize);
				if(buffer1 < buffer2)
				{
					buffer1 = buffer2;
					k = j;
					flag = BlockofMemory[j];
				}
			}
			buffer1 = 0;
			buffer2 = 0;
			BlockofMemory[k] = PageNumofR[i];
			count+=1;
			flag = OutBlockofM(BlockofMemory,BlockSize,flag);
		}
		else
			flag = OutBlockofM(BlockofMemory,BlockSize,flag);
	}
	printf("OPT缺页率为：%.3f\n",(float)count/(count+BlockSize));
	printf("抖动的次数为：%d\n",count);
	QuitBofM(BlockofMemory,BlockSize);
}

//先进先出页面置换算法
void Fifo(int *BlockofMemory,int *PageNumofR,int BlockSize,int PageNumRefSize,int flag,int count)
{
	int i;
	printf("**********先进先出页面置换算法：**********\n");
	//输出页面引用串号
	OutPageNumofR(PageNumofR,PageNumRefSize);
	
	//输出内存块页面序号
	for(i = 0; i < BlockSize; i++)
	{
		BlockofMemory[i] = PageNumofR[i];
		flag = OutBlockofM(BlockofMemory,BlockSize,flag);
	}
	for(i = BlockSize; i < PageNumRefSize; i++)
	{
		if(!PageInBofM(PageNumofR[i],BlockofMemory,BlockSize))//页不在内存中
		{
			flag = BlockofMemory[count%BlockSize];
			BlockofMemory[count%BlockSize] = PageNumofR[i];
			count+=1;
			flag = OutBlockofM(BlockofMemory,BlockSize,flag);
		}
		else
			flag = OutBlockofM(BlockofMemory,BlockSize,flag);
	}
	printf("FIFO缺页率为：%.2f\n",(float)count/(count+BlockSize));
	printf("抖动的次数为：%d\n",count);
	QuitBofM(BlockofMemory,BlockSize);
}

int main()
{
        /* 注意：在应用程序中不能使用断点等调试功能 */
	int i;
	int BlockSize;			//内存块大小
	int PageNumRefSize;		//页面号引用串大小
	int *BlockofMemory;		//内存物理块
	int flag = -1;			//标志位，内存块是否改变
	int count = 0;			//缺页数
	int PageNumofR[] = {7,0,1,2,0,3,0,4,2,3,0,3,2,1,2,0,1,1,7,0,1};  //页面号引用串

	BlockSize = BLOCKSIZE;
	BlockofMemory = (int*)malloc(BlockSize * sizeof(int));
	if(BlockofMemory == (int*)NULL)
	{
		printf("内存分配出错\n");
		exit(1);
	}
	//初始化内存物理块
	for(i = 0; i < BlockSize; i++)
		BlockofMemory[i] = 0;
	
	PageNumRefSize = PAGENUMOFRSIZE;

	Opt(BlockofMemory, PageNumofR, BlockSize, PageNumRefSize, flag, count);
	Fifo(BlockofMemory, PageNumofR, BlockSize, PageNumRefSize, flag, count);

	free(BlockofMemory);
	free(PageNumofR);
	
	return 0;
}
