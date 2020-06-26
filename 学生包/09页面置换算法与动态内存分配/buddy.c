#include <stdio.h>
#include <math.h>

#define RED FOREGROUND_RED
#define GREEN FOREGROUND_GREEN
#define BLUE FOREGROUND_BLUE

#define M 10	/* 可利用空间总容量(2的幂次)，子表的个数为M+1 */
#define N 100	/* 占用块个数的最大值 */

//伙伴系统可利用空间表的结构
typedef struct _Node{
	struct _Node *llink;	//指向前驱节点
	int bflag;				//块标志，0：空闲，1：占用
	int bsize;				//块大小，值为2的幂次k
	struct _Node *rlink;	//头部域，指向后继节点
}Node;

//可利用空间表的头结点
typedef struct HeadNode{
	int nodesize;		//链表的空闲块大小
	Node *first;		//链表的表头指针
}FreeList[M+1];			//表头向量类型

Node* start;   // 全局变量start为整个生成空间的首地址，

/* avail[0...M]为可利用空间表，n为申请分配量。若有不小于n的空闲块，
则分配相应的存储块，并返回首地址；否则，返回NULL */
Node* AllocBuddy(FreeList *avail, double n)
{
	int i, k;
	Node *pa, *pi, *pre, *suc;
	
	//查找满足分配要求的子表即空闲块大小大于n的表
	
	//此处代码需读者自己设计实现

	if(k>M) 	//	分配失败返回NULL
	{
		printf("内存分配失败！\n");
		return NULL;
	}
	else{		//进行分配
		pa = (*avail)[k].first;			//指向可分配表的第一个节点
		pre = pa->llink;				//分别指向前驱和后继
		suc = pa->rlink;				
		
		if(pa == suc)					//分配后该子表变成空表
			(*avail)[k].first = NULL;
			
		else{							//从子表删去*pa节点
			pre->rlink = suc;
			suc->llink = pre;
			(*avail)[k].first = suc;
		}
		
		/* 将剩余块插入相应子表 */
		for(i = 1; (*avail)[k-i].nodesize >= n+1; ++i)
		{
			
			//此处代码需读者自己设计实现
			
		}
		pa->bflag = 1;
		pa->bsize = k - (--i);
	}
	return pa;
}

//返回相对起始地址为p，块大小为pow(2,k)的块的伙伴地址
Node* buddy(Node* n)
{
	if((n-start)%(int)pow(2,n->bsize+1)==0)		//p为前块
		return n + (int)pow(2,n->bsize);
	else										//p为后块
		return n - (int)pow(2,n->bsize);
}

// 伙伴系统的回收算法 将p所指的释放块回收到可利用空间表pav中
void Reclaim(FreeList *pav,Node* *p)
{ 
	Node* s;

	// 伙伴块的起始地址
	s = buddy(*p); 
	// 归并伙伴块
	while(s >= start && s < start + (*pav)[M].nodesize && s->bflag == 0)
	{	//伙伴块起始地址在有效范围内且伙伴块空闲 
		// 从链表上删除该结点 
		if(s->llink == s && s->rlink == s)	// 链表上仅此一个结点 
			(*pav)[s->bsize].first = NULL;	// 置此链表为空 
		else	// 链表上不止一个结点，则将该结点s删除 
		{
			
			//此处代码需读者自己设计实现
			
		} 
		// 修改结点头部 
		if( (*p-start) % (int)pow(2,(*p)->bsize+1) == 0) // p为前块 
			(*p)->bsize++;
		else // p为后块 
		{
			s->bsize = (*p)->bsize+1;
			*p = s; // p指向新块首地址 
		}
		s = buddy(*p); // 下一个伙伴块的起始地址 
		// printf("伙伴块的起始地址：%u\n",s);
	}
	// 将p插到可利用空间表中 
	(*p)->bflag = 0;
	if((*pav)[(*p)->bsize].first == NULL) // 该链表空 
		(*pav)[(*p)->bsize].first = (*p)->llink = (*p)->rlink = *p;
	else // 插在表头 
	{
		
		//此处代码需读者自己设计实现
		
	}
	*p = NULL;
}

/* 设置字体颜色 */
void SetColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_INTENSITY | color);
}

//输出所有可利用空间表
void print_free(FreeList p)
{
	int i;
	Node* head;
	SetColor(RED|BLUE|GREEN);
	printf("可利用空间：\n");
	printf("      块的大小    块的首地址\t块标志(0:空闲 1:占用)\n");
	
	SetColor(GREEN);
											
	for(i = 0; i <= M; i++)
	{
		if(p[i].first)		//第i个可利用空间表不空
		{
			
			//此处代码需读者自己设计实现
			
		}
	}
	printf("\n");
}

// 输出p数组所指的已分配空间
void print_used(Node* p[])
{ 
	int i;
	
	SetColor(RED|BLUE|GREEN);
	printf("已利用空间：\n");
	printf("     占用块块号\t占用块的首地址\t块大小\t块标志(0:空闲 1:占用)\n");
	
	SetColor(RED);
	
	for(i = 0; i < N; i++)
		if(p[i]) // 指针不为0(指向一个占用块) 
			printf("\t  %d\t   %u\t %d\t  %d\n",i, p[i],(int)pow(2,p[i]->bsize), p[i]->bflag); 
	printf("\n");
}

int main(int argc, char* argv[])
{
    /* 注意：在应用程序中不能使用断点等调试功能 */
	int i, j, item;
	float joblength;
	FreeList avail;
	Node* used_block[N] = {NULL};	//used_block数组为占用块的首地址
	
	//初始化可利用空间
	for(i = 0; i <= M; i++)
	{
		avail[i].nodesize = (int)pow(2,i);
		avail[i].first = NULL;
	}
	
	// 在最大链表中生成一个结点 
	start = avail[M].first = (Node*)malloc(avail[M].nodesize * sizeof(Node));
	if(start) // 生成结点成功 
	{
		start->llink = start->rlink = start; // 初始化该结点 
		start->bflag = 0;
		start->bsize = M;
		
		i = 0;
		while(1)
		{
			//设置字体颜色为白色
			SetColor(RED|BLUE|GREEN);
			printf("选择功能项：(0-退出  1-分配主存  2-回收主存  3-显示主存):");
			scanf("%d",&item);
			switch(item)
			{
				case 0:
					exit(0);
				case 1:
					SetColor(RED|GREEN);
					printf("输入作业所需长度：");
					scanf("%ld",&joblength);
					used_block[i] = AllocBuddy(&avail,joblength);
					++i;
					break;
				case 2:
					SetColor(RED|BLUE);
					printf("输入要回收块的块号：");
					scanf("%d",&j);
					Reclaim(&avail,&used_block[j]);
					break;
				case 3:	
					print_free(avail);
					print_used(used_block);
					break;
				default:
					printf("没有该选项\n");
			}
		}

	}
	
}
