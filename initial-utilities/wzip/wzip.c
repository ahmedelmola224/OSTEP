#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int counter=1;
    char *line=NULL,last=0;
    size_t size=0;
    int i=1;
     if(argc<2)
    {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }
     while(i<argc)
    {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL)
    	{
	
			printf("wzip: cannot open file\n");
			exit(1);
        }

        while(getline(&line, &size,fp)!=EOF)
        {
           
            if(last==line[0])
            {
                counter++;
            }
            else if(counter>=1&&last)
            {
                fwrite(&counter, sizeof(int), 1, stdout);
                fwrite(&last, sizeof(char), 1, stdout);
                last=0;
                counter=1;
            }

            for(int j=0;line[j];j++)
            {
                if(line[j]==line[j+1])
                {    
                    counter++;
                }
                else if((feof(fp)&&i<argc-1)||(line[j]=='\n'))
                {
                    last=line[j];
                }
                else
                {                    
                    fwrite(&counter, sizeof(int), 1, stdout);
                    fwrite(line+j, sizeof(char), 1, stdout);
                    counter=1;

                }
            }   
            free(line);
            line=NULL;
            size=0;
        }

      if(last=='\n')
      {
        fwrite(&counter, sizeof(int), 1, stdout);
        fwrite(&last, sizeof(char), 1, stdout);
      }
        
        i++;

    }
    return 0;
}