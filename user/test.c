#include "../kernel/pstat.h"
#include "./user.h"

void run()
{
	while(1);
	exit();
}
struct pstat p = {0} ;
int main()
{
	int i;
	int x = getpinfo(&p);
  	if (x < 0)
  	{
    		exit();
  	}
	int PID_1=fork();
	if(PID_1==0)
	{
		settickets(10);
		run();
	}

	int PID_2=fork();
	if(PID_2==0)
	{
		settickets(20);
		run();
	}
	int PID_3=fork();
	if(PID_3==0)
	{
		settickets(30);
		run();
	}
 // Wait for the child processes to exit
  	for (i = 0; i < 3; i++) 
	{
    		wait();
  	}

  exit();	
}
