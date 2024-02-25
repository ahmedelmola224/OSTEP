#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *line=NULL;
    size_t size=0;
    int i =2;
    if(argc<2)
    {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }
    if(argc==2)
    {
         while(getline(&line, &size,stdin)!=-1)
        {
            if(strstr(line, argv[1])!=NULL)
            {
                printf("%s",line);
            }

        }
    }
    while(i<argc)
    {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL)
    	{
	
			printf("wgrep: cannot open file\n");
			exit(1);
        }
        while(getline(&line, &size,fp)!=-1)
        {
            if(strstr(line, argv[1])!=NULL)
            {
                printf("%s",line);
            }
            free(line);
            line=NULL;
            size=0;
        }
        i++;

    }
    free(line);

    return 0;

}