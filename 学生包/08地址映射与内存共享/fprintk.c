#include<linux/sched.h>
#include<sys/stat.h>

static char logbuf[1024];

int fprintk( int fd, const char * fmt, ... )
{
	va_list args;
	int i;
	struct file * file;
	struct m_inode * inode;
	va_start (args, fmt);		
  	i = vsprintf (logbuf, fmt, args);
  	va_end (args);
  	
  	if( fd<3 )
  	{
  		__asm__ ("push %%fs\n\t"		
	   	"push %%ds\n\t"
	   	"pop %%fs\n\t"	
	  	"pushl %0\n\t"					
	   	"pushl $_logbuf\n\t"			
	   	"pushl %1\n\t"					
	   	"call _sys_write\n\t"			
	   	"addl $8,%%esp\n\t"				
	   	"popl %0\n\t"					
	   	"pop %%fs"						
		::"r" (i),"r" (fd):"ax",  "dx");
  	}
  	else
  	{
  		if( !( file=task[0]->filp[fd] ) )
  			return 0;
  		inode=file->f_inode;
  		__asm__ ("push %%fs\n\t"		
	   	"push %%ds\n\t"
	   	"pop %%fs\n\t"	
	  	"pushl %0\n\t"					
	   	"pushl $_logbuf\n\t"			
	   	"pushl %1\n\t"					
	   	"pushl %2\n\t"					
	   	"call _file_write\n\t"			
	   	"addl $12,%%esp\n\t"			
	   	"popl %0\n\t"					
	   	"pop %%fs"						
		::"r" (i), "r" (file), "r" (inode)   );	
  	}
  	return i;// ·µ»Ø×Ö·û´®³¤¶È
}

