//最近最久未使用页面置换算法
void Lru(int *BlockofMemory,int *PageNumRef,int BlockSize,int PageNumRefSize,int flag,int count)
{
	int i,j,k=0;
	int buffer1 = 0,buffer2 = 0;
	printf("************最近最久未使用页面置换算法：************\n");
	
	//输出页面引用串号
	OutPageNumofR(PageNumRef,PageNumRefSize);
	for(i = 0; i < BlockSize; i++)
	{
		//此处代码需读者自己完成
	}
	for(i = BlockSize; i < PageNumRefSize; i++)
	{
		if(页不在内存中)
		{
		        //求出最长最近时间不被访问的页
			for(j = 0; j < BlockSize; j++)
			{
				//此处代码需读者自己完成
			}
			buffer1 = 0;
			buffer2 = 0;
			
			//内存块页块号赋值为页面引用串号
			//抖动次数计数值加1
			//输出内存块页面序号
			
			//此处代码需读者自己完成
			
		}
		//页在内存中
		else 
                     //输出内存块页面序号
                    
	}
	printf("LRU缺页率为：%.2f\n",(float)count/(count+BlockSize));
	printf("抖动的次数为：%d\n",count);
	
	//内存块页面序号清零
	QuitBofM(BlockofMemory,BlockSize);
}

//简单的Clock页面置换算法
void Clock(int *BlockofMemory,int *PageNumRef,int BlockSize,int PageNumRefSize,int flag,int count)
{
	int* Interview;//访问位，没必要为每页设置，只为每内存块设置一位即可
	int i, j = 0, k;
	Interview = (int*)malloc(BlockSize*sizeof(int));
	if(Interview == (int*)NULL)
	{
		printf("内存分配出错\n");
		exit(1);
	}
	printf("************简单的Clock页面置换算法：************\n");
	OutPageNumofR(PageNumRef,PageNumRefSize);
	/* 输出内存块页面号 */
	for(i = 0; i < BlockSize; i++)
	{
		//此处代码需读者自己完成
	}
	
	for(i = BlockSize; i < PageNumRefSize; i++)
	{
		if(页不在内存中)
		{
			for(k = 0; k < BlockSize; k++)
			{
				//判断访问位是否为0，如果为0，跳出循环，否则访问位置0
				
				//此处代码需读者自己完成
				
				j++;
				if(j == BlockSize)
					j=0;
			}
			
			//访问位置1
			//标志位置为内存块中j偏移量位的数据
			//内存块j偏移量位赋值为页面引用块号
			//抖动次数计数值加1
			//输出内存块页面序号
			
			//此处代码需读者自己完成
		}
		//页在内存中
		else
			//输出内存块页面序号
			
	}
	printf("CLOCK缺页率为：%.2f\n",(float)count/(count+BlockSize));
	printf("抖动的次数为：%d\n",count);
	//内存块页面序号清零
	QuitBofM(BlockofMemory,BlockSize);
	free(Interview);
}
