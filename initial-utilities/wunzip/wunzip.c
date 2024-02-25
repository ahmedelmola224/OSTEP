#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    int i=1;
    int n=0;
    size_t elements_read=0;
    char data[5];
    if(argc<2)
    {
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }
    while(i<argc)
    {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL)
    	{
	
			printf("wunzip: cannot open file\n");
			exit(1);
        }
        while ((elements_read = fread(data, 1, sizeof(data), fp)) > 0)
        {
            n=data[0]|(data[1]<<(int)8)|(data[2]<<(int)16)|(data[3]<<(int)24);
            for(int j=0;j<n;j++)
            {
                printf("%c",data[4]);
            }
        }
        i++;
    }


    return 0;
}