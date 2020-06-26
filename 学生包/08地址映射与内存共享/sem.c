#define __LIBRARY__
#include <unistd.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <asm/system.h>
#include <asm/segment.h>
#include <signal.h>
#include <string.h>

#define ENOMEM 12
#define EINVAL 22
int vector[20]={0};

#define SEM_MAX 		20	// ����ź�������
#define NAME_MAX		16	// �ź������Ƶ���󳤶�

// ���������������
typedef struct _item
{
	struct task_struct* task;	// ��ǰ����ָ��
	struct _item* next;			// ��һ���������ָ��
}item;

// �����ź����ṹ��
typedef struct _sem_t
{
	char sem_name[NAME_MAX];	// �ź���������

	int value;  // ������Դ������
	int used;	// �ź��������ü�����Ϊ0��ʾδʹ�ã�����0��ʾ��ʹ��
	item* wait;  // �������ź����µĵȴ�������
}sem_t;

sem_t sem_array[SEM_MAX];  // ϵͳ�д���ź����Ľṹ������


// �������������ȴ����еĶ�β
void wait_task(struct task_struct* task, sem_t* sem)
{
	item** end;
	item* new_item;
	
	// �����������β�������һ������
	for(end = &sem->wait; *end != NULL; end = &(*end)->next)
		;
	
	// �½�item�ڵ㣬���½��ڵ���뵽��������item��β��	
	new_item = (item*)malloc(sizeof(item));
	new_item->task = task;
	new_item->next = NULL;
	*end = new_item;

	return;
}

// �������ź����в����ź���
int find_sem(const char* name)
{
	int i;
	for(i = 0; i < SEM_MAX; i++)
	{
		// ����������ź����д��ڵ�ǰ�ź������򷵻ص�ǰ�ź������ź��������е�ƫ����
		if(sem_array[i].used > 0 && 0 == strcmp(sem_array[i].sem_name, name))
		{
			return i;
		}
	}
	// �����ź�����û�е�ǰ�źţ�����-1
	return -1;
}

// ���û������user_name���ݵ��ںˣ��ٷ����ں���kernel_name�ĵ�ַ
char* get_name(const char* user_name)
{
	static char kernel_name[NAME_MAX]={};
	char temp;
	int i = 0;
	
	while((temp = get_fs_byte(user_name + i)) != '\0') 
	{	// get_fs_byte()���������������ں˿ռ�ȡ���û��ռ������
		kernel_name[i] = temp;
		i++;
	}
	kernel_name[i] = '\0';
	
	return kernel_name;
}

// �����ź���
sem_t* sys_sem_open(const char* name, unsigned int value)
{
	int i = 0;
	char* kernel_name;
	
	cli();		// ���ж�
	kernel_name = get_name(name);  /* kernel_nameָ����get_name()�����ڶ����static����kernel_nameָ����ڴ� */
	
	// ����������ź������ҵ���ǰ�ź������򷵻��ź��������еĵ�ǰ�ź���
	if (-1 != (i = find_sem(kernel_name)))
	{
		sem_array[i].used++;
		
		sti();		// ���ж�
		return &sem_array[i];
	}
	
	// �������ź���δ�ҵ���ǰ�ź������򴴽��µ��ź���
	for (i = 0; i < SEM_MAX; i++)
	{
		if(0 == sem_array[i].used)
		{
			strcpy(sem_array[i].sem_name, kernel_name);
			sem_array[i].value = value;
			sem_array[i].used++;
			sem_array[i].wait->next = NULL;		// ��ʼ���ź����������
			sti();		// ���ж�
			return &sem_array[i];
		}
	}
	sti();		// ���ж�
	return &sem_array[SEM_MAX];
}


// �ȴ��ź���
int sys_sem_wait(sem_t* sem)
{
	cli();		// ���ж�
 	sem->value--;
 	if(sem->value < 0)
 	{
 		wait_task(current, sem);
		current->state = TASK_UNINTERRUPTIBLE;
		schedule();
	}
	sti();		// ���ж�
	return 0;
}

// �ͷ��ź���
 int sys_sem_post(sem_t* sem)
 {
 	struct task_struct* p;
 	item* first;
 	
	cli();		// ���ж�
 	sem->value++;
	if(sem->value <= 0 && sem->wait != NULL)
	{
		first = sem->wait;
		
		p = first->task;
		p->state = TASK_RUNNING;
		
		sem->wait = first->next;
		free(first);		
	}
	sti();		//���ж�
	return 0;
}

// �ر��ź���
int sys_sem_unlink(const char *name)
{
	int locate = 0;
	char* kernel_name;
	
	cli(); 		// ���ж�
	kernel_name = get_name(name); /* Tempnameָ����get_name()�����ڶ����static����tempnameָ����ڴ� */
	
	if (-1 != (locate = find_sem(kernel_name)))  //�����ź����д��ڵ�ǰ�ź���
	{
		// �������ʹ�õ�ǰ�ź��������ź�������ֵ��һ
		sem_array[locate].used--;
		sti();		// ���ж�
 		return 0;
 	}
 	sti();  // ���ж�
	return -1;
}

/* �½�/��һҳ�����ڴ棬�����ظ�ҳ�����ڴ��shmid���ÿ鹲���ڴ��ڲ���ϵͳ�ڲ���id����*/
int sys_shmget(int key, int size)
{
	int free = 0;
	
	if(vector[key] != 0)
	{
		return vector[key];
	}

	if(size > 4096) 
		return -EINVAL;
		
	free = get_free_page();		// get_free_page()���ص�������Ŀ���ҳ��ĵ�ַ
	if(!free)
	{
		return -ENOMEM;
	}
	
	vector[key] = free;		
	return vector[key];
}

/* ��shmidָ���Ĺ���ҳ��ӳ�䵽��ǰ���̵������ַ�ռ��У��������׵�ַ���ء� */
void* sys_shmat(int shmid, const void *shmaddr)
{
	if(!shmid) 
		return NULL;
	put_page(shmid, current->start_code + current->brk);
	return (void*)current->brk;
}
