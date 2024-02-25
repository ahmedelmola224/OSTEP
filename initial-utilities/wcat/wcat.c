#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER 512
int main(int argc, char *argv[])
{
	if(argc > 1)
	{
		int i=1;
		char line[MAX_BUFFER];
		while(i<argc)
		{
			
    		FILE *fp = fopen(argv[i], "r");
    		if (fp == NULL)
    		{
	
				printf("wcat: cannot open file\n");
    			exit(1);
			}
			while(fgets(line,MAX_BUFFER, fp)!=NULL)
			{
				printf("%s",line);
			}
			i++;
		}
	}

    return 0;

}
