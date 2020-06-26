#include <stdio.h>
#include <windows.h>

#define RED FOREGROUND_RED
#define GREEN FOREGROUND_GREEN
#define BLUE FOREGROUND_BLUE

#define ADDRESS		10240
#define LENGTH		102400

#define N 10  /* ����ϵͳ����������ҵ�� */
#define M 10  /* ����ϵͳ����Ŀ����������ֵ */
#define MINISIZE 100	/* ���û���Ҫ�Ŀռ��ϵͳ���п������С���Ǻܽӽ�ʱ��ϵͳ���ռ������û���
						ϵͳʣ��Ŀ��п����������С�����ɴη���֮��ϵͳ�оͻ����һЩ����
						��С��Ҳ���䲻��ȥ�Ŀ��п飬��ʹ������˷��䣨���ң����ٶȡ�����˺궨
						�峣����Ŀ�ľ���Ϊ���ֲ���ȱ�ݣ���ϵͳ������û��ռ��ʣ���ڴ�С�ڴ�ֵ��
						��ʣ���ڴ�Ҳ������û�����֮��ֻ������û�������ڴ�*/

/* �����ṹ�� */
struct table{
	unsigned long address;	/* �ڴ���ʼ��ַ */
	unsigned long length;	/* �ڴ泤�ȣ���λΪ�ֽ� */
	int flag;		/* 0��ʾ����Ŀ��1��ʾδʹ�ÿռ� */
};

struct table used_table[N] = {0};	/* �����ѷ������� */
struct table free_table[M] = {0};	/* ����������� */


/* �����״���Ӧ�㷨����length��С���ڴ� */
void Allocate(unsigned long length)
{
	int i, k;
	unsigned long ad;
	unsigned long start;
	k = -1;
	
	/* ����free_table���ҿռ����length����С�������Ǽ���k */
	for( i = 0; i < M; i++)
	{
		if( (free_table[i].flag == 1)&&(free_table[i].length >= length) )
		{
			if((k == -1) || free_table[i].length < free_table[k].length)
				k = i;
		}
	}
	if(k == -1){
		printf("�޿��ÿ�����\n");
		return ;
	}
	/* �ҵ����ÿ���������ʼ���䡣
	����������С��Ҫ�����ռ��С��MINISIZE��С���������ȫ�����䣻
	����������С��Ҫ�����Ŀռ�����MINISIZE��С����ӿ���������һ���ַ��� */		
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
	
	/* �޸��ѷ������� */
	i =0;			
	while(used_table[i].flag != 0 && i<N)
		i++;
	if(i>=N) //�ޱ�Ŀ��д�ѷ�����
	{
		printf("�ѷ������������޷����������ڴ棡\n");
		if(free_table[k].flag == 0)  //������������
			free_table[k].flag = 1;	//ǰ���ҵ������������з���
		else	//ǰ���ҵ�����ĳ�����з�����һ����
		{
			free_table[k].length = free_table[k].length + length;
			return ;
		}
	}
	else  /* �޸��ѷ�������used_table */
	{
		used_table[i].address = ad;
		used_table[i].length = length;
		used_table[i].flag = 1;
	}
	return;
}

/* �ڴ���պ��� */
void Reclaim(unsigned long start)
{
	int i, j, k, s, t;
	unsigned long S, L;
	s = 0;

	if(start < ADDRESS || start > (ADDRESS + LENGTH) )
	{
		printf("����ĵ�ַ����\n");
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
			printf("�����Ҫ���շ������׵�ַ��Ҫ�����ѷ�����д��ڵ��׵�ַ��\n");
			return;
		}
	}

	/* �޸��ѷ�������used_table */
	used_table[s].flag = 0;
	// ȡ�ù黹��������ʼ��ַS�ͳ���L
	S = used_table[s].address;
	L = used_table[s].length;
	used_table[s].address = 0;
	used_table[s].length = 0;
	
	j = -1;
	k = -1;
	i = 0;

	// Ѱ�һ��շ����Ŀ��������ڣ����ڱ�Ŀk,���ڱ�Ŀj
	while(i<M && (j==-1 || k==-1))
	{
		if(free_table[i].flag == 1)
		{
			if(free_table[i].address + free_table[i].length == S)/* �ҵ����� */
				k = i;
			if(free_table[i].address == S+L) /* �ҵ����� */
				j = i;
		}
		i++;
	}
	if(-1 != k)
	{
		if(-1 != j)//���ڿ����������ڿ������������ϲ�
		{
			free_table[k].length = free_table[j].length + free_table[k].length + L;
			free_table[j].flag = 0;
		}
		else //���ڿ����������ڷǿ������������ںϲ�
			free_table[k].length = free_table[k].length + L;
	}
	else if(j != -1) //���ڷǿ����������ڿ������������ںϲ�
	{
		free_table[j].address = S;
		free_table[j].length = free_table[j].length + L;
	}
	else // �����ھ�Ϊ�ǿ�������������ֱ������
	{
		t = 0;
		while(free_table[t].flag == 1 && t<M)
			t++;
		if(t>=M)
		{
			printf("�ڴ���б�û�пռ䣬���տռ�ʧ��\n");
			used_table[s].flag = 1;
			return;
		}
		free_table[t].address = S;
		free_table[t].length = L;
		free_table[t].flag = 1;
	}
	return;	
}

/* ����������ɫ */
void SetColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_INTENSITY | color);
}

int main(int argc, char* argv[])
{
        /* ע�⣺��Ӧ�ó����в���ʹ�öϵ�ȵ��Թ��� */
	int i, j, item;
	unsigned long joblength;
	unsigned long start;

	/* ���������ʼ�� */
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
		printf("ѡ�����(0-�˳�  1-�����ڴ�  2-�����ڴ�  3-��ʾ�ڴ�)��");
		scanf("%d",&item);
		switch(item)
		{
			case 0:
				exit(0);
			case 1:
				SetColor(RED|GREEN);
				printf("�����ڴ泤�ȣ�");
				scanf("%*c%ld",&joblength);
				Allocate(joblength);
				break;
			case 2:
				SetColor(RED|BLUE);
				printf("����Ҫ���շ������׵�ַ��");
				scanf("%*c%ld",&start);
				Reclaim(start);
				break;
			case 3:
				SetColor(RED|GREEN|BLUE);
				printf("��������\n��ʼ��ַ �������� ��־\n");
				SetColor(GREEN);
				for(i=0; i<M; i++)
					printf("%6ld%9ld%6d\n",free_table[i].address,free_table[i].length,free_table[i].flag);
				
				SetColor(RED|GREEN|BLUE);
				printf("���������ַ�����ѷ����...\n");
				getch();
				printf("�ѷ�������\n��ʼ��ַ ��������\n");	
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
				printf("û�и�ѡ��\n");
		}
	}
}
