void physical_mem()
{
	int i,free=0;	//free表示空闲页的总数
	unsigned long address;

	__asm("cli");
	
	/* PAGING_PAGES是在本文件第68行处定义，表示分页后的物理内存页数。*/
	/* mem_map[]是在第83行定义的内存映射字节图(1 字节代表1 页内存)，
		每个页面对应的字节用于标志页面当前被引用（占用）次数。*/
	for(i=0 ; i<PAGING_PAGES ; i++)
		if (!mem_map[i]) free++;
	printk("Page Count : %d\n",PAGING_PAGES);
	printk("Memory Count : %d * 4096 = %d Byte\n\n",PAGING_PAGES, PAGING_PAGES * 4096);
	
	printk("Free Page Count : %d\n",free);
	printk("Used Page Count : %d\n",PAGING_PAGES - free);
	

	printk("**************After Allocate One Page******************\n");
	address = get_free_page();
	for(i=0, free=0 ; i<PAGING_PAGES ; i++)
		if (!mem_map[i]) free++;
	
	printk("Free Page Count : %d\n",free);
	printk("Used Page Count : %d\n",PAGING_PAGES - free);
	
	
	printk("*****************After Free One Page*******************\n");
	free_page(address);
	for(i=0, free=0 ; i<PAGING_PAGES ; i++)
		if (!mem_map[i]) free++;
	
	printk("Free Page Count : %d\n",free);
	printk("Used Page Count : %d\n",PAGING_PAGES - free);
	
	__asm("sti");
}

