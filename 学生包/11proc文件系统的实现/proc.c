#include <asm/segment.h>
#include <linux/sched.h>

#define  SIZE  1024		//缓冲区大小
char psbuffer[SIZE];	//定义缓冲区来存放结点信息

/* 将整型数据转换为字符数据并赋值给psbuffer */
void itoa(int num, int* j) 
{
	char buffer[16] = {};	//定义一个缓冲区来临时存放将整型num转换成的反向的字符串
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
	
	//将整型数据num转换成反向字符串，保存到buffer中
	//i同时表示num转换成的字符的个数
	for(i = 0; num != 0; i++)
	{
		buffer[i] = (char)(num % 10 + '0'); 	//将num的个位数转换成字符数据放到buffer中
		num = num / 10;		//去掉个位数
	}
	
	//反向将buffer中的字符放入到psbuffer中
	//数组下标是此0开始计数的，所以首先需要将i减1
	for(--i; i >= 0; i--)
	{
		psbuffer[*j] = buffer[i];
		(*j) ++;
	}
	psbuffer[*j] = '\t';		//此处加制表符是为来显示时将数据对齐，以便于查看
	(*j)++;
}

/* 向数组psbuffer中添加标题 */
/* 参数j为在psbuffer中写数据的偏移位置 */
void addTitle(char* title, int* j)
{
	//这里也可以使用strcat，但是由于*j的值还需要来记录偏移的位置，这样就
	//还需要使用strlen来计算*title的长度，效率可能会降低许多，
	//同时代码量也不会比下面使用的代码量少
	for( ; *title; (*j)++, *title++)
		psbuffer[*j] = *title;	//逐字节的将标题添加到psbuffer中
	
	psbuffer[*j] = '\t';		//此处加制表符是为来显示时将数据对齐，以便于查看
	(*j) ++;
}

/*  读取系统中的信息放到psbuffer数组中,并再读取psbuffer中的数据写入到用户
	空间的buf中。读取位置从f_pos指向的位置开始，每次最多读count个字节，并根据
	实际读到的字节数调整f_pos的值，最后返回实际读到的字节数*/
int psread(int dev,char * buf,int count,off_t * f_pos)
{
	struct task_struct * p;
	int i, j = 0;	//变量j为在psbuffer中写数据的位置偏移量
	
	//第一次从psbuffers数组的起始位置0开始，读取完毕后第二次再进入到此函数就
	//不会再为psbuffer赋值，直接执行后面的for循环体
	if(!(*f_pos))	
	{
		memset(psbuffer, 0, SIZE);	//将psbuffer数组初始化为0
		
		//添加psinfo节点的标题
		addTitle("pid", &j);
		addTitle("state", &j);
		addTitle("father", &j);
		addTitle("counter", &j);
		addTitle("start_time", &j);
		psbuffer[j] = '\n';
		j++;
		
		//遍历当前的进程，为psinfo节点赋值
		//NR_TASKS为系统最大的任务数的宏定义
		for(i = 0; i < NR_TASKS; i++)
		{
			p = task[i]; 
			if(p == NULL)continue;
			
			//添加psinfo节点下的数据
			itoa( p->pid, &j);
			itoa( p->state, &j);
			itoa( p->father, &j);
			itoa( p->counter, &j);
			itoa( p->start_time, &j);
			psbuffer[j] = '\n';
			j++;
		}
	}

	//将数组psbuffer中的信息读取到用户空间的buf位置处
	for(i = 0; i < count; i++, (*f_pos)++)
	{
		if(psbuffer[*f_pos] == '\0')
			break;
		//put_fs_byte函数是将一字节存放在 fs 段中指定内存地址处。
		//其第一个参数是char类型，第二个参数是char*类型
		put_fs_byte(psbuffer[*f_pos], &buf[i]);
	}
	
	return i;
}
