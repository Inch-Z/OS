void physical_mem()
{
	int i,free=0;	//free��ʾ����ҳ������

	__asm("cli");
	
	/* PAGING_PAGES���ڱ��ļ���68�д����壬��ʾ��ҳ��������ڴ�ҳ����*/
	/* mem_map[]���ڵ�83�ж�����ڴ�ӳ���ֽ�ͼ(1 �ֽڴ���1 ҳ�ڴ�)��
		ÿ��ҳ���Ӧ���ֽ����ڱ�־ҳ�浱ǰ�����ã�ռ�ã�������*/
	for(i=0 ; i<PAGING_PAGES ; i++)
		if (!mem_map[i]) free++;
	printk("Page Count : %d\n",PAGING_PAGES);
	printk("Memory Count : %d * 4096 = %d Byte\n\n",PAGING_PAGES, PAGING_PAGES * 4096);
	
	printk("Free Page Count : %d\n",free);
	printk("Used Page Count : %d\n",PAGING_PAGES - free);
	
	__asm("sti");
}

