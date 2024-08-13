#include "user.h"
#include "kernel/pstat.h"
#include "kernel/types.h"
int main()
{
	struct pstat p ;
 	 if (getpinfo(&p) < 0) 
	 {
    		exit();
  	 }
  	for (int i = 0 ; i < NPROC ; i++)
	{      if (p.inuse[i] == 1)
    		{
      		 printf("pid is %d \n",p.pid[i]);
      		 printf("tickets are %d \n",p.tickets[i]);
      		 printf("ticks are %d \n",p.ticks[i]);
      		 printf(".........................\n");
    		}
  	}
	printf("...................................\n");
  exit();
}
