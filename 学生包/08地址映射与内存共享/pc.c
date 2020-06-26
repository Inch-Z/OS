#define __LIBRARY__
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
	
#define BUFFER_SIZE 10	/* ���������� */
#define NUM 25			/* ��Ʒ���� */

#define __NR_sem_open 87
#define __NR_sem_wait 88
#define __NR_sem_post 89
#define __NR_sem_unlink 90

typedef void sem_t;  /* �����õ�sem_t*�ģ���ʵ�ʶ���ָ�룬���ڴ˽�sem_t*���Ͷ���Ϊvoid*���ͣ�
						�Ͳ����ڴ����¶���sem_t�Ľṹ�壬���Ҳ�����˶Խṹ��Ĺ��������
						�����¶���˽ṹ�壬�ں���Ҳ�Ѿ������˴˽ṹ�壬��������޸ģ���ͬʱ
						��Ҫ�޸Ĵ˴����ڴ洦�����Ľṹ�� */

_syscall2(int, sem_open, const char*, name, unsigned int, value)
_syscall1(int, sem_wait, sem_t *, sem)
_syscall1(int, sem_post, sem_t *, sem)
_syscall1(int, sem_unlink, const char *, name)

int main()
{	
	/* ע��: ��Ӧ�ó����в���ʹ�öϵ�ȵ��Թ��� */
	int i, j;
	int consumeNum = 0; /* ���������ѵĲ�Ʒ�� */
	int produceNum = 0; /* �����������Ĳ�Ʒ�� */
	int consume_pos = 0; /* �����ߴӹ���������ȡ����Ʒ���ѵ�λ�� */
	int produce_pos = 0; /* ������������Ʒ�������������λ�� */
	
	sem_t *empty, *full, *mutex;
	FILE *fp = NULL;
	pid_t producer_pid, consumer_pid;
	
	/* ����empty��full��mutex�����ź��� */
	empty = (sem_t*)sem_open("empty", BUFFER_SIZE);
	full  = (sem_t*)sem_open("full", 0);
	mutex = (sem_t*)sem_open("mutex", 1);
	
	/* ���ļ�����һ���������� */
	fp=fopen("filebuffer.txt", "wb+");
	
	/* ���������߽��� */
	if( !fork() )
	{
		producer_pid = getpid();
		printf("producer %d create success!\n", producer_pid);
		for( i = 0 ; i < NUM; i++)
		{
			sem_wait(empty);
			sem_wait(mutex);
			
			produceNum = i;
			
			/* �ƶ��ļ����α꣬�����з����Ʒ */
			fseek(fp, produce_pos * sizeof(int), SEEK_SET);
			fwrite(&produceNum, sizeof(int), 1, fp);
			fflush(fp); 
			
			/* ���������Ʒ����Ϣ */
			printf("Producer %d : %02d at %d\n", producer_pid, produceNum, produce_pos); 
			fflush(stdout);
			
			/* �����ߵ��α�����ƶ�һ��λ�� */
			produce_pos = (produce_pos + 1) % BUFFER_SIZE;
			
			sem_post(mutex);
			sem_post(full);
			
			sleep(2);
		}
		exit(0);
	}
	
	/* ���������߽��� */
	if( !fork() )
	{
		consumer_pid = getpid();
		printf("\t\t\tconsumer %d create success!\n", consumer_pid);
		for( j = 0; j < NUM; j++ ) 
		{
			sem_wait(full);
			sem_wait(mutex);
			
			/* �ƶ��ļ����α꣬ȡ�����еĲ�Ʒ */
			fseek(fp, consume_pos * sizeof(int), SEEK_SET);
			fread(&consumeNum, sizeof(int), 1, fp);
			fflush(fp);
			
			/* ������Ѳ�Ʒ����Ϣ */
			printf("\t\t\tConsumer %d: %02d at %d\n", consumer_pid, consumeNum, consume_pos);
			fflush(stdout);
			
			/* �����ߵ��α�����ƶ�һ��λ�� */
			consume_pos = (consume_pos + 1) % BUFFER_SIZE;
	
			sem_post(mutex);
			sem_post(empty);
			
			if(j<4)	sleep(8);
			else sleep(1);
		}
		exit(0);
	}

	waitpid(producer_pid, NULL, 0);	/* �ȴ������߽��̽��� */
	waitpid(consumer_pid, NULL, 0);	/* �ȴ������߽��̽��� */
	
	/* �ر������ź��� */
	sem_unlink("empty");
	sem_unlink("full");
	sem_unlink("mutex");
	
	/* �ر��ļ� */
	fclose(fp);
	
	return 0;
}
