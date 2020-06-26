/* �������̨Ӧ�ó������ڵ� */

#include <stdio.h>

#define PHYSICAL_BLOCK_SIZE 1024
#define LOGICAL_BLOCK_SIZE 1024
#define NAME_LEN 14
#define START_PARTITION_TABLE 0x1be


//������ṹ
struct par_table_entry {
	char boot_indicator;	//����ָʾ������������������0
	char start_chs_val[3];	//��ʼ�����ͷ����ֵ��3���ֽڷֱ��Ӧ����š���ͷ�š�������
	char par_type;			//��������
	char end_chs_val[3];	//��ֹ�����ͷ����ֵ
	int start_sector;		//��ʼ�̿��
	int par_size;			//������С
};

// ������ṹ��
struct super_block
{
  unsigned short s_ninodes;		// �ڵ�����
  unsigned short s_nzones;		// �߼�������
  unsigned short s_imap_blocks;	// i �ڵ�λͼ��ռ�õ����ݿ�����
  unsigned short s_zmap_blocks;	// �߼���λͼ��ռ�õ����ݿ�����
  unsigned short s_firstdatazone;	// ��һ�������߼���š�
  unsigned short s_log_zone_size;	// log(���ݿ���/�߼���)������2 Ϊ�ף���
  unsigned long s_max_size;		// �ļ���󳤶ȡ�
  unsigned short s_magic;		// �ļ�ϵͳħ����
};

// i�ڵ�ṹ��
struct d_inode
{
  unsigned short i_mode;		// �ļ����ͺ�����(rwx λ)��
  unsigned short i_uid;			// �û�id���ļ�ӵ���߱�ʶ������
  unsigned long i_size;			// �ļ���С���ֽ�������
  unsigned long i_time;			// �޸�ʱ�䣨��1970.1.1:0 �����룩��
  unsigned char i_gid;			// ��id(�ļ�ӵ�������ڵ���)��
  unsigned char i_nlinks;		// �����������ٸ��ļ�Ŀ¼��ָ���i �ڵ㣩��
  unsigned short i_zone[9];		// ֱ��(0-6)�����(7)��˫�ؼ��(8)�߼���š�
								// zone ��������˼����������Σ����߼��顣
};

//Ŀ¼��ṹ
struct dir_entry{
	unsigned short inode;	//i�ڵ��
	char name[NAME_LEN];	//�ļ���
};

struct super_block sblock;		//������
struct par_table_entry pte[4];	//����������
FILE* fd;						//�ļ�ָ��
char physical_block[PHYSICAL_BLOCK_SIZE]; //�洢�����
char logical_block[LOGICAL_BLOCK_SIZE];  //�洢�߼���
char *inode_bitmap;		//i�ڵ�λͼָ��
//char *logical_bitmap;	//�߼���λͼָ��,��ʵ��δʹ��

//��ȡһ�������
void get_physical_block(int block_num)
{
	//��1����Ϊ�����̿��Ǵ�1��ʼ����
	fseek(fd, (block_num - 1) * PHYSICAL_BLOCK_SIZE, SEEK_SET);
	fread(physical_block, PHYSICAL_BLOCK_SIZE, 1, fd);
}

//��ȡ��һ��������һ���߼���
void get_partition_logical_block(int block_num)
{
	//block_numǰ��ӵ�1��ʾ�ڵ�һ������ǰ����һ����������¼��MBR���飬
	//����ӵ�1����Ϊ�����̿��Ǵ�1��ʼ������,���߼����Ǵ�0��ʼ������
	get_physical_block(1 + block_num + 1);
	memcpy(logical_block, physical_block, LOGICAL_BLOCK_SIZE);
}

//��ȡ������
void get_partition_table()
{
	int i = 0;

	//��������4��16�ֽڵı�����ɣ���һ���������ʼ��ַΪSTART_PARTITION_TABLE
	get_physical_block( 1 );	//�������������̿�ĵ�1��
	memcpy(pte, &physical_block[START_PARTITION_TABLE], sizeof(pte));
	for(i = 0; i < 4; i++)
	{
		printf("**************pattition table%d****************\n", i+1);
		printf("Boot Indicator:%d\n", pte[i].boot_indicator);
		printf("start CHS value:0x%04x\n", pte[i].start_chs_val);
		printf("partition type:%ld\n", pte[i].par_type);
		printf("end CHS value:0x%04x\n", pte[i].end_chs_val);
		printf("start sector:%d\n", pte[i].start_sector);
		printf("partition size:%d\n", pte[i].par_size);
	}
}

//��ȡ��һ�������ĳ�����
void get_super_block()
{	
	get_partition_logical_block( 1 );
	memcpy(&sblock, logical_block, sizeof(sblock));
	
	printf("**************super block****************\n");
	printf("ninodes��%d\n", sblock.s_ninodes);
	printf("nzones��%d\n", sblock.s_nzones);
	printf("imap_blocks��%d\n", sblock.s_imap_blocks);
	printf("zmap_blocks��%d\n", sblock.s_zmap_blocks);
	printf("firstdatazone��0x%04x\n", sblock.s_firstdatazone);
	printf("log_zone_size��%d\n", sblock.s_log_zone_size);
	printf("max_size��0x%x = %dByte\n", sblock.s_max_size,sblock.s_max_size);
	printf("magic��0x%x\n", sblock.s_magic);
}	


//����i�ڵ�λͼ
void load_inode_bitmap()
{
	inode_bitmap = (char*)malloc(sblock.s_imap_blocks * LOGICAL_BLOCK_SIZE);
	int i = 0;
	for(i = 0; i < sblock.s_imap_blocks; i++)
	{
		//i�ڵ�λͼǰ��1���������һ��������
		get_partition_logical_block(1 + 1 + i);	
		memcpy(&inode_bitmap[i * LOGICAL_BLOCK_SIZE], &logical_block, LOGICAL_BLOCK_SIZE);
	}
}

//����i�ڵ�λͼ�ж����Ӧ��i�ڵ��Ƿ���Ч
//����inode_idΪi�ڵ��id
//��Ч����1����Ч����0
int is_inode_valid(unsigned short inode_id)
{
	if(inode_id > sblock.s_ninodes)
		return 0;
		
	char byte = inode_bitmap[(inode_id - 1) / 8]; //inode_id��1����Ϊi�ڵ��Ǵ�1��ʼ������
	return (byte >> (7 - (inode_id - 1) % 8) ) & 0x1;	//ȡһ���ֽ��е�ĳλ��1��λ����
}

//����i�ڵ�id��ȡi�ڵ�
void get_inode(unsigned short inode_id, struct d_inode* inode)
{
	//һ�������飬һ�������飬sblock.s_imap_blocks��i�ڵ�λͼ��sblock.s_zmap_blocks���߼���λͼ	
	//һ��i�ڵ�ռ32���ֽڣ�һ���̿���LOGICAL_BLOCK_SIZE/32���ڵ㣬����inode_id/(LOGICAL_BLOCK_SIZE/32)
	//��1����Ϊi�ڵ���Ǵ�1��ʼ�����ģ����߼�����Ǵ�0��ʼ������
	//inode_blocknum��i�ڵ����߼����е�ƫ�ƿ���
	int inode_blocknum = 1 + 1 + sblock.s_imap_blocks + sblock.s_zmap_blocks + (inode_id - 1) / (LOGICAL_BLOCK_SIZE/32) ;
	get_partition_logical_block(inode_blocknum);
	memcpy((char*)inode, &logical_block[((inode_id - 1) % sizeof(struct d_inode)) * sizeof(struct d_inode)], sizeof(struct d_inode));
}

//�ݹ��ӡi�ڵ��µ�Ŀ¼
void print_inode(unsigned short id, int tab_count, const char* name)
{
	int i, m, n;
	struct d_inode inode;
	struct dir_entry dir;
	
	//���i�ڵ�Ŷ�Ӧ��i�ڵ�λͼ��Ӧλ��ֵΪ1,˵����i�ڵ���ʹ��
	//����˵����i�ڵ����û��ѱ�ɾ������ֱ�ӷ���
	if(is_inode_valid(id) != 1)
		return;
		
	get_inode(id, &inode);
	tab_count++;
	unsigned short mode = inode.i_mode >> 12; //��4λ��ŵ����ļ�����
	
	//�����Ŀ¼�ļ�
	if(mode == 4)
	{
		//��ӡtab����Ϊ��ʹĿ¼�в�θ�
		for(i=0; i<tab_count; i++)
		{
			printf("\t");
		}
		printf("%s\n", name);
		
		//ѭ����ȡi�ڵ��е�i_zones[]����
		for(m = 0; m<7; m++)
		{
			//�����������Ϊ0��������
			if(inode.i_zone[m] == 0)
				continue;
			
			//һ���߼������洢64��Ŀ¼��,ѭ����ȡ64��Ŀ¼��
			//����ǰ����ֱ�Ϊ . �� .. 
			for(n = 0; n < 64; n++)
			{
				get_partition_logical_block(inode.i_zone[m]);
				//���߼����е����ݿ�����Ŀ¼��ṹ��
				memcpy((char*)&dir, &logical_block[n * sizeof(dir)], sizeof(dir));
				
				//����� .��..�����ѭ��
				if(n == 0 || n == 1)
					continue;
					
				//���Ŀ¼����û�����������ٶ�ȡ
				if(dir.inode == 0)
					break;
				
				//�ݹ��ӡ��Ŀ¼
				print_inode(dir.inode, tab_count, dir.name);
			}
		}
	}
	
	//����ǳ����ļ�
	else if(mode == 8)
	{
		for(i=0; i<tab_count; i++)
		{
			printf("\t");
		}
		printf("%s\n", name);
	}
	//������豸�ļ����ַ��豸�ļ������������ļ�������߳����Լ�ʵ��
}


int main(int argc, char* argv[])
{
	int bit;
	struct d_inode* inode = (struct d_inode*)malloc(sizeof(struct d_inode));
	
	char* path = "C:\\minix\\harddisk.img";
	fd = fopen(path, "rb");
	if(fd==NULL)
		printf("open file failed!\n");
		
	//��ȡ������
	get_partition_table();
	//��ȡ������
	get_super_block();
	
	//����i�ڵ��߼���λͼ
	load_inode_bitmap();
	//i�ڵ�λͼ�ĵ�һλ��Ӧ�ļ�ϵͳ�ĸ��ڵ�
	//�����һλΪ1�����ӡ���ڵ�
	bit = is_inode_valid(1);
	if(bit == 1)
		print_inode(1, -1, "\\");
	else
		printf("root node lost��\n");
	
	return 0;
}


