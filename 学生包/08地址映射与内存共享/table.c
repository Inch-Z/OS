extern void	calc_mem();
void sys_table_mapping()
{
	unsigned long index_of_dir,  index_of_table;
	unsigned long entry;
	unsigned long page_table_base;
	unsigned long page_dir_base = 0;

	__asm("cli");
	
	calc_mem();	// ��ʾ�ڴ����ҳ������
	//��һ��ѭ��������ҳĿ¼�е� PDE
	for(index_of_dir = 0; index_of_dir < 1024; index_of_dir++)
	{
		entry = ((unsigned long*)page_dir_base)[index_of_dir];
		
		if(!(entry & 1))	/* ������Ч�� PDE */
			continue;
		
		//��� PDE ��Ϣ,��ʽ���£�  
		//PDE: ��� (ӳ��� 4M �����ַ�Ļ�ַ)->��ӳ��ҳ���ҳ���
		fprintk(1,"PDE: 0x%X (0x%X)->0x%X\n", index_of_dir, index_of_dir << 22, entry >> 12);

		/* ҳĿ¼��ĸ�20λ��Ϊҳ��������ַ��ҳ��������ַ��Ϊҳ��������ַ */
		page_table_base = 0xFFFFF000 & entry;
		
		/*�ڶ���ѭ��������ҳ���е� PTE*/
		for(index_of_table = 0; index_of_table < 1024; index_of_table++)
		{
			entry = ((unsigned long*)page_table_base)[index_of_table];
			
			if(!(entry & 1))	/* ������Ч�� PTE */
				continue;	
			
			// ��� PTE ��Ϣ����ʽ���£�
			// PTE: ��� (ӳ��� 4K �����ַ�Ļ�ַ)->��ӳ������ҳ��ҳ���
			fprintk(1,"\t\tPTE: 0x%X (0x%X)->0x%X\n", index_of_table, 
				(index_of_dir << 22) | (index_of_table << 12), entry >> 12);
		}
	 }
	__asm("sti");

}
