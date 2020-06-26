extern void	calc_mem();
void sys_table_mapping()
{
	unsigned long index_of_dir,  index_of_table;
	unsigned long entry;
	unsigned long page_table_base;
	unsigned long page_dir_base = 0;

	__asm("cli");
	
	calc_mem();	// 显示内存空闲页面数。
	//第一层循环，遍历页目录中的 PDE
	for(index_of_dir = 0; index_of_dir < 1024; index_of_dir++)
	{
		entry = ((unsigned long*)page_dir_base)[index_of_dir];
		
		if(!(entry & 1))	/* 跳过无效的 PDE */
			continue;
		
		//输出 PDE 信息,格式如下：  
		//PDE: 标号 (映射的 4M 虚拟地址的基址)->所映射页表的页框号
		fprintk(1,"PDE: 0x%X (0x%X)->0x%X\n", index_of_dir, index_of_dir << 22, entry >> 12);

		/* 页目录项的高20位即为页表的物理地址，页表的物理地址即为页表的虚拟地址 */
		page_table_base = 0xFFFFF000 & entry;
		
		/*第二层循环，遍历页表中的 PTE*/
		for(index_of_table = 0; index_of_table < 1024; index_of_table++)
		{
			entry = ((unsigned long*)page_table_base)[index_of_table];
			
			if(!(entry & 1))	/* 跳过无效的 PTE */
				continue;	
			
			// 输出 PTE 信息，格式如下：
			// PTE: 标号 (映射的 4K 虚拟地址的基址)->所映射物理页的页框号
			fprintk(1,"\t\tPTE: 0x%X (0x%X)->0x%X\n", index_of_table, 
				(index_of_dir << 22) | (index_of_table << 12), entry >> 12);
		}
	 }
	__asm("sti");

}
