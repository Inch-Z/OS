#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>

#define BLOCKSIZE 5			//�ڴ���С
#define PAGENUMOFRSIZE 15	//ҳ�����ô���С

//����ڴ��ҳ�����
int OutBlockofM(int *BlockofMemory, int BlockSize, int flag)
{
	int i;
	printf("�ڴ���ҳ��Ϊ��\t");
	for(i = 0; i < BlockSize; i++)
		printf("%d  ",BlockofMemory[i]);
	printf("\t");
	if(flag != -1)
		printf("��̭ҳ���Ϊ��%d",flag);
	printf("\n");
	return -1;
}

//���ҳ�����ô���
void OutPageNumofR(int* PageNumofR, int PageNumRefSize)
{
	int i = 0;
	printf("ҳ�����ô��ǣ�\t");
	for(i = 0; i < PageNumRefSize; i++)
		printf("%d  ",PageNumofR[i]);
	printf("\n");
}

//�ڴ��ҳ���������
void QuitBofM(int *BlockofMemory, int BlockSize)
{
	int i;
	for(i = 0; i < BlockSize; i++)
		BlockofMemory[i] = 0;
}

//�ж�ҳ�Ƿ����ڴ��У����ҳ���ڴ��У�����1�����򷵻�0��
int PageInBofM(int PageNum,int *BlockofMemory,int BlockSize)
{
	int i;
	for(i = 0; i < BlockSize; i++)
		if(PageNum == BlockofMemory[i])
			return 1;
	return 0;
}

//�´η��ʴ���
//����j:  ҳ�����ڴ���е�λ��
//����i�� ҳ�����ҳ������ô��е�λ��
int DistanceOtp(int *BlockofMemory, int *PageNumofR, int j, int i, int PageNumRefSize)
{
	int k;
	for(k = i + 1; k < PageNumRefSize; k++)
		if(BlockofMemory[j] == PageNumofR[k])
			return k;
	return PageNumRefSize;
}

//���һ�η��ʴ���
//����j:  ҳ�����ڴ���е�λ��
//����i�� ҳ�����ҳ������ô��е�λ��
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

//���ҳ���û��㷨
void Opt(int *BlockofMemory,int *PageNumofR,int BlockSize,int PageNumRefSize,int flag,int count)
{
	int i, j, k = 0;
	int buffer1 = 0, buffer2 = 0;
	printf("**********���ҳ���û��㷨��**********\n");
	//���ҳ�����ô���
	OutPageNumofR(PageNumofR, PageNumRefSize);
	
	//����ڴ��ҳ�����
	for(i = 0; i < BlockSize; i++)
	{
		BlockofMemory[i] = PageNumofR[i];
		flag = OutBlockofM(BlockofMemory,BlockSize,flag);
	}
	for(i = BlockSize;i < PageNumRefSize; i++)
	{
		if(!PageInBofM(PageNumofR[i], BlockofMemory, BlockSize))//ҳ�����ڴ���
		{
			//����δ��ʱ�䲻�����ʵ�ҳ
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
	printf("OPTȱҳ��Ϊ��%.3f\n",(float)count/(count+BlockSize));
	printf("�����Ĵ���Ϊ��%d\n",count);
	QuitBofM(BlockofMemory,BlockSize);
}

//�Ƚ��ȳ�ҳ���û��㷨
void Fifo(int *BlockofMemory,int *PageNumofR,int BlockSize,int PageNumRefSize,int flag,int count)
{
	int i;
	printf("**********�Ƚ��ȳ�ҳ���û��㷨��**********\n");
	//���ҳ�����ô���
	OutPageNumofR(PageNumofR,PageNumRefSize);
	
	//����ڴ��ҳ�����
	for(i = 0; i < BlockSize; i++)
	{
		BlockofMemory[i] = PageNumofR[i];
		flag = OutBlockofM(BlockofMemory,BlockSize,flag);
	}
	for(i = BlockSize; i < PageNumRefSize; i++)
	{
		if(!PageInBofM(PageNumofR[i],BlockofMemory,BlockSize))//ҳ�����ڴ���
		{
			flag = BlockofMemory[count%BlockSize];
			BlockofMemory[count%BlockSize] = PageNumofR[i];
			count+=1;
			flag = OutBlockofM(BlockofMemory,BlockSize,flag);
		}
		else
			flag = OutBlockofM(BlockofMemory,BlockSize,flag);
	}
	printf("FIFOȱҳ��Ϊ��%.2f\n",(float)count/(count+BlockSize));
	printf("�����Ĵ���Ϊ��%d\n",count);
	QuitBofM(BlockofMemory,BlockSize);
}

int main()
{
        /* ע�⣺��Ӧ�ó����в���ʹ�öϵ�ȵ��Թ��� */
	int i;
	int BlockSize;			//�ڴ���С
	int PageNumRefSize;		//ҳ������ô���С
	int *BlockofMemory;		//�ڴ������
	int flag = -1;			//��־λ���ڴ���Ƿ�ı�
	int count = 0;			//ȱҳ��
	int PageNumofR[] = {7,0,1,2,0,3,0,4,2,3,0,3,2,1,2,0,1,1,7,0,1};  //ҳ������ô�

	BlockSize = BLOCKSIZE;
	BlockofMemory = (int*)malloc(BlockSize * sizeof(int));
	if(BlockofMemory == (int*)NULL)
	{
		printf("�ڴ�������\n");
		exit(1);
	}
	//��ʼ���ڴ������
	for(i = 0; i < BlockSize; i++)
		BlockofMemory[i] = 0;
	
	PageNumRefSize = PAGENUMOFRSIZE;

	Opt(BlockofMemory, PageNumofR, BlockSize, PageNumRefSize, flag, count);
	Fifo(BlockofMemory, PageNumofR, BlockSize, PageNumRefSize, flag, count);

	free(BlockofMemory);
	free(PageNumofR);
	
	return 0;
}
