#include <asm/segment.h>
#include <linux/sched.h>

#define  SIZE  1024		//��������С
char psbuffer[SIZE];	//���建��������Ž����Ϣ

/* ����������ת��Ϊ�ַ����ݲ���ֵ��psbuffer */
void itoa(int num, int* j) 
{
	char buffer[16] = {};	//����һ������������ʱ��Ž�����numת���ɵķ�����ַ���
	int i = 0;
 	if (num == 0)
 	{
		psbuffer[*j] = '0';
		(*j)++;
		psbuffer[*j] = '\t';
		(*j)++;
		return;
	}
	if (num<0)
	{
		psbuffer[*j] = '-';
		(*j)++;
		num = -num;
	}
	
	//����������numת���ɷ����ַ��������浽buffer��
	//iͬʱ��ʾnumת���ɵ��ַ��ĸ���
	for(i = 0; num != 0; i++)
	{
		buffer[i] = (char)(num % 10 + '0'); 	//��num�ĸ�λ��ת�����ַ����ݷŵ�buffer��
		num = num / 10;		//ȥ����λ��
	}
	
	//����buffer�е��ַ����뵽psbuffer��
	//�����±��Ǵ�0��ʼ�����ģ�����������Ҫ��i��1
	for(--i; i >= 0; i--)
	{
		psbuffer[*j] = buffer[i];
		(*j) ++;
	}
	psbuffer[*j] = '\t';		//�˴����Ʊ����Ϊ����ʾʱ�����ݶ��룬�Ա��ڲ鿴
	(*j)++;
}

/* ������psbuffer����ӱ��� */
/* ����jΪ��psbuffer��д���ݵ�ƫ��λ�� */
void addTitle(char* title, int* j)
{
	//����Ҳ����ʹ��strcat����������*j��ֵ����Ҫ����¼ƫ�Ƶ�λ�ã�������
	//����Ҫʹ��strlen������*title�ĳ��ȣ�Ч�ʿ��ܻή����࣬
	//ͬʱ������Ҳ���������ʹ�õĴ�������
	for( ; *title; (*j)++, *title++)
		psbuffer[*j] = *title;	//���ֽڵĽ�������ӵ�psbuffer��
	
	psbuffer[*j] = '\t';		//�˴����Ʊ����Ϊ����ʾʱ�����ݶ��룬�Ա��ڲ鿴
	(*j) ++;
}

/*  ��ȡϵͳ�е���Ϣ�ŵ�psbuffer������,���ٶ�ȡpsbuffer�е�����д�뵽�û�
	�ռ��buf�С���ȡλ�ô�f_posָ���λ�ÿ�ʼ��ÿ������count���ֽڣ�������
	ʵ�ʶ������ֽ�������f_pos��ֵ����󷵻�ʵ�ʶ������ֽ���*/
int psread(int dev,char * buf,int count,off_t * f_pos)
{
	struct task_struct * p;
	int i, j = 0;	//����jΪ��psbuffer��д���ݵ�λ��ƫ����
	
	//��һ�δ�psbuffers�������ʼλ��0��ʼ����ȡ��Ϻ�ڶ����ٽ��뵽�˺�����
	//������Ϊpsbuffer��ֵ��ֱ��ִ�к����forѭ����
	if(!(*f_pos))	
	{
		memset(psbuffer, 0, SIZE);	//��psbuffer�����ʼ��Ϊ0
		
		//���psinfo�ڵ�ı���
		addTitle("pid", &j);
		addTitle("state", &j);
		addTitle("father", &j);
		addTitle("counter", &j);
		addTitle("start_time", &j);
		psbuffer[j] = '\n';
		j++;
		
		//������ǰ�Ľ��̣�Ϊpsinfo�ڵ㸳ֵ
		//NR_TASKSΪϵͳ�����������ĺ궨��
		for(i = 0; i < NR_TASKS; i++)
		{
			p = task[i]; 
			if(p == NULL)continue;
			
			//���psinfo�ڵ��µ�����
			itoa( p->pid, &j);
			itoa( p->state, &j);
			itoa( p->father, &j);
			itoa( p->counter, &j);
			itoa( p->start_time, &j);
			psbuffer[j] = '\n';
			j++;
		}
	}

	//������psbuffer�е���Ϣ��ȡ���û��ռ��bufλ�ô�
	for(i = 0; i < count; i++, (*f_pos)++)
	{
		if(psbuffer[*f_pos] == '\0')
			break;
		//put_fs_byte�����ǽ�һ�ֽڴ���� fs ����ָ���ڴ��ַ����
		//���һ��������char���ͣ��ڶ���������char*����
		put_fs_byte(psbuffer[*f_pos], &buf[i]);
	}
	
	return i;
}
