//������δʹ��ҳ���û��㷨
void Lru(int *BlockofMemory,int *PageNumRef,int BlockSize,int PageNumRefSize,int flag,int count)
{
	int i,j,k=0;
	int buffer1 = 0,buffer2 = 0;
	printf("************������δʹ��ҳ���û��㷨��************\n");
	
	//���ҳ�����ô���
	OutPageNumofR(PageNumRef,PageNumRefSize);
	for(i = 0; i < BlockSize; i++)
	{
		//�˴�����������Լ����
	}
	for(i = BlockSize; i < PageNumRefSize; i++)
	{
		if(ҳ�����ڴ���)
		{
		        //�������ʱ�䲻�����ʵ�ҳ
			for(j = 0; j < BlockSize; j++)
			{
				//�˴�����������Լ����
			}
			buffer1 = 0;
			buffer2 = 0;
			
			//�ڴ��ҳ��Ÿ�ֵΪҳ�����ô���
			//������������ֵ��1
			//����ڴ��ҳ�����
			
			//�˴�����������Լ����
			
		}
		//ҳ���ڴ���
		else 
                     //����ڴ��ҳ�����
                    
	}
	printf("LRUȱҳ��Ϊ��%.2f\n",(float)count/(count+BlockSize));
	printf("�����Ĵ���Ϊ��%d\n",count);
	
	//�ڴ��ҳ���������
	QuitBofM(BlockofMemory,BlockSize);
}

//�򵥵�Clockҳ���û��㷨
void Clock(int *BlockofMemory,int *PageNumRef,int BlockSize,int PageNumRefSize,int flag,int count)
{
	int* Interview;//����λ��û��ҪΪÿҳ���ã�ֻΪÿ�ڴ������һλ����
	int i, j = 0, k;
	Interview = (int*)malloc(BlockSize*sizeof(int));
	if(Interview == (int*)NULL)
	{
		printf("�ڴ�������\n");
		exit(1);
	}
	printf("************�򵥵�Clockҳ���û��㷨��************\n");
	OutPageNumofR(PageNumRef,PageNumRefSize);
	/* ����ڴ��ҳ��� */
	for(i = 0; i < BlockSize; i++)
	{
		//�˴�����������Լ����
	}
	
	for(i = BlockSize; i < PageNumRefSize; i++)
	{
		if(ҳ�����ڴ���)
		{
			for(k = 0; k < BlockSize; k++)
			{
				//�жϷ���λ�Ƿ�Ϊ0�����Ϊ0������ѭ�����������λ��0
				
				//�˴�����������Լ����
				
				j++;
				if(j == BlockSize)
					j=0;
			}
			
			//����λ��1
			//��־λ��Ϊ�ڴ����jƫ����λ������
			//�ڴ��jƫ����λ��ֵΪҳ�����ÿ��
			//������������ֵ��1
			//����ڴ��ҳ�����
			
			//�˴�����������Լ����
		}
		//ҳ���ڴ���
		else
			//����ڴ��ҳ�����
			
	}
	printf("CLOCKȱҳ��Ϊ��%.2f\n",(float)count/(count+BlockSize));
	printf("�����Ĵ���Ϊ��%d\n",count);
	//�ڴ��ҳ���������
	QuitBofM(BlockofMemory,BlockSize);
	free(Interview);
}
