/* 定义控制台应用程序的入口点 */

#include <stdio.h>

#define PHYSICAL_BLOCK_SIZE 1024
#define LOGICAL_BLOCK_SIZE 1024
#define NAME_LEN 14
#define START_PARTITION_TABLE 0x1be


//分区表结构
struct par_table_entry {
	char boot_indicator;	//导入指示器，绝大多数情况都是0
	char start_chs_val[3];	//起始柱面磁头扇区值，3个字节分别对应柱面号、磁头号、扇区号
	char par_type;			//分区类型
	char end_chs_val[3];	//终止柱面磁头扇区值
	int start_sector;		//起始盘块号
	int par_size;			//分区大小
};

// 超级块结构体
struct super_block
{
  unsigned short s_ninodes;		// 节点数。
  unsigned short s_nzones;		// 逻辑块数。
  unsigned short s_imap_blocks;	// i 节点位图所占用的数据块数。
  unsigned short s_zmap_blocks;	// 逻辑块位图所占用的数据块数。
  unsigned short s_firstdatazone;	// 第一个数据逻辑块号。
  unsigned short s_log_zone_size;	// log(数据块数/逻辑块)。（以2 为底）。
  unsigned long s_max_size;		// 文件最大长度。
  unsigned short s_magic;		// 文件系统魔数。
};

// i节点结构体
struct d_inode
{
  unsigned short i_mode;		// 文件类型和属性(rwx 位)。
  unsigned short i_uid;			// 用户id（文件拥有者标识符）。
  unsigned long i_size;			// 文件大小（字节数）。
  unsigned long i_time;			// 修改时间（自1970.1.1:0 算起，秒）。
  unsigned char i_gid;			// 组id(文件拥有者所在的组)。
  unsigned char i_nlinks;		// 链接数（多少个文件目录项指向该i 节点）。
  unsigned short i_zone[9];		// 直接(0-6)、间接(7)或双重间接(8)逻辑块号。
								// zone 是区的意思，可译成区段，或逻辑块。
};

//目录项结构
struct dir_entry{
	unsigned short inode;	//i节点号
	char name[NAME_LEN];	//文件名
};

struct super_block sblock;		//超级块
struct par_table_entry pte[4];	//分区表数组
FILE* fd;						//文件指针
char physical_block[PHYSICAL_BLOCK_SIZE]; //存储物理块
char logical_block[LOGICAL_BLOCK_SIZE];  //存储逻辑块
char *inode_bitmap;		//i节点位图指针
//char *logical_bitmap;	//逻辑块位图指针,本实例未使用

//读取一个物理块
void get_physical_block(int block_num)
{
	//减1是因为物理盘块是从1开始计数
	fseek(fd, (block_num - 1) * PHYSICAL_BLOCK_SIZE, SEEK_SET);
	fread(physical_block, PHYSICAL_BLOCK_SIZE, 1, fd);
}

//读取第一个分区的一个逻辑块
void get_partition_logical_block(int block_num)
{
	//block_num前面加的1表示在第一个分区前还有一个主引导记录（MBR）块，
	//后面加的1是因为物理盘块是从1开始计数的,而逻辑块是从0开始计数的
	get_physical_block(1 + block_num + 1);
	memcpy(logical_block, physical_block, LOGICAL_BLOCK_SIZE);
}

//读取分区表
void get_partition_table()
{
	int i = 0;

	//分区表有4个16字节的表项组成，第一个表项的起始地址为START_PARTITION_TABLE
	get_physical_block( 1 );	//分区表在物理盘块的第1块
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

//读取第一个分区的超级块
void get_super_block()
{	
	get_partition_logical_block( 1 );
	memcpy(&sblock, logical_block, sizeof(sblock));
	
	printf("**************super block****************\n");
	printf("ninodes：%d\n", sblock.s_ninodes);
	printf("nzones：%d\n", sblock.s_nzones);
	printf("imap_blocks：%d\n", sblock.s_imap_blocks);
	printf("zmap_blocks：%d\n", sblock.s_zmap_blocks);
	printf("firstdatazone：0x%04x\n", sblock.s_firstdatazone);
	printf("log_zone_size：%d\n", sblock.s_log_zone_size);
	printf("max_size：0x%x = %dByte\n", sblock.s_max_size,sblock.s_max_size);
	printf("magic：0x%x\n", sblock.s_magic);
}	


//加载i节点位图
void load_inode_bitmap()
{
	inode_bitmap = (char*)malloc(sblock.s_imap_blocks * LOGICAL_BLOCK_SIZE);
	int i = 0;
	for(i = 0; i < sblock.s_imap_blocks; i++)
	{
		//i节点位图前有1个引导块和一个超级块
		get_partition_logical_block(1 + 1 + i);	
		memcpy(&inode_bitmap[i * LOGICAL_BLOCK_SIZE], &logical_block, LOGICAL_BLOCK_SIZE);
	}
}

//根据i节点位图判断其对应的i节点是否有效
//参数inode_id为i节点的id
//有效返回1，无效返回0
int is_inode_valid(unsigned short inode_id)
{
	if(inode_id > sblock.s_ninodes)
		return 0;
		
	char byte = inode_bitmap[(inode_id - 1) / 8]; //inode_id减1是因为i节点是从1开始计数的
	return (byte >> (7 - (inode_id - 1) % 8) ) & 0x1;	//取一个字节中的某位与1做位运算
}

//根据i节点id读取i节点
void get_inode(unsigned short inode_id, struct d_inode* inode)
{
	//一个引导块，一个超级块，sblock.s_imap_blocks个i节点位图，sblock.s_zmap_blocks个逻辑块位图	
	//一个i节点占32个字节，一个盘块有LOGICAL_BLOCK_SIZE/32个节点，所以inode_id/(LOGICAL_BLOCK_SIZE/32)
	//减1是因为i节点号是从1开始计数的，而逻辑块号是从0开始计数的
	//inode_blocknum是i节点在逻辑块中的偏移块数
	int inode_blocknum = 1 + 1 + sblock.s_imap_blocks + sblock.s_zmap_blocks + (inode_id - 1) / (LOGICAL_BLOCK_SIZE/32) ;
	get_partition_logical_block(inode_blocknum);
	memcpy((char*)inode, &logical_block[((inode_id - 1) % sizeof(struct d_inode)) * sizeof(struct d_inode)], sizeof(struct d_inode));
}

//递归打印i节点下的目录
void print_inode(unsigned short id, int tab_count, const char* name)
{
	int i, m, n;
	struct d_inode inode;
	struct dir_entry dir;
	
	//如果i节点号对应在i节点位图相应位的值为1,说明此i节点已使用
	//否则说明此i节点无用或已被删除，则直接返回
	if(is_inode_valid(id) != 1)
		return;
		
	get_inode(id, &inode);
	tab_count++;
	unsigned short mode = inode.i_mode >> 12; //高4位存放的是文件类型
	
	//如果是目录文件
	if(mode == 4)
	{
		//打印tab键，为了使目录有层次感
		for(i=0; i<tab_count; i++)
		{
			printf("\t");
		}
		printf("%s\n", name);
		
		//循环读取i节点中的i_zones[]数组
		for(m = 0; m<7; m++)
		{
			//如果数组数据为0，则跳过
			if(inode.i_zone[m] == 0)
				continue;
			
			//一个逻辑块最多存储64个目录项,循环读取64个目录项
			//其中前两项分别为 . 和 .. 
			for(n = 0; n < 64; n++)
			{
				get_partition_logical_block(inode.i_zone[m]);
				//将逻辑块中的数据拷贝到目录项结构体
				memcpy((char*)&dir, &logical_block[n * sizeof(dir)], sizeof(dir));
				
				//如果是 .和..则继续循环
				if(n == 0 || n == 1)
					continue;
					
				//如果目录项中没有内容了则不再读取
				if(dir.inode == 0)
					break;
				
				//递归打印子目录
				print_inode(dir.inode, tab_count, dir.name);
			}
		}
	}
	
	//如果是常规文件
	else if(mode == 8)
	{
		for(i=0; i<tab_count; i++)
		{
			printf("\t");
		}
		printf("%s\n", name);
	}
	//如果块设备文件、字符设备文件等其他类型文件，请读者尝试自己实现
}


int main(int argc, char* argv[])
{
	int bit;
	struct d_inode* inode = (struct d_inode*)malloc(sizeof(struct d_inode));
	
	char* path = "C:\\minix\\harddisk.img";
	fd = fopen(path, "rb");
	if(fd==NULL)
		printf("open file failed!\n");
		
	//读取分区表
	get_partition_table();
	//读取超级块
	get_super_block();
	
	//加载i节点逻辑块位图
	load_inode_bitmap();
	//i节点位图的第一位对应文件系统的根节点
	//如果第一位为1，则打印根节点
	bit = is_inode_valid(1);
	if(bit == 1)
		print_inode(1, -1, "\\");
	else
		printf("root node lost！\n");
	
	return 0;
}


