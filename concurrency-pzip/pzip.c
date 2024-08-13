#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <pthread.h>

#define DEFAULT_CHUNCK_SIZE 1024 
#define MAX_NUMBER_OF_CHUNKS 10000
#define MAX_NUMBER_OF_FILES 30
typedef struct 
{
  int chunkId;                
  int chunkSize;              
  char* chunkStarting;                  
  char *chunkEncodedInMemory ; 
  char* chunkCursor;
  char chunkIsLast;
  char lastInFile;   
  char*chunkfree;  
}chunk;

typedef struct 
{               
  int fileId;   
  size_t fileSize;  
  size_t fileSizeConst;            
  char *fileInMemory ;
  char *fileCursor;        
}file;


pthread_mutex_t file_lock = PTHREAD_MUTEX_INITIALIZER ;     // lock for the file 
pthread_mutex_t chunks_lock = PTHREAD_MUTEX_INITIALIZER ;   // lock for the chunks queue 
pthread_cond_t fill_chunks_cv = PTHREAD_COND_INITIALIZER ;  // to wake the consumer threads to dequeue 
pthread_cond_t empty_chunks_cv = PTHREAD_COND_INITIALIZER ; // to wake the producer thread to enqueue

/*zip*/
chunk*zippedData[MAX_NUMBER_OF_CHUNKS];


/*queue elements*/
unsigned int fill = 0 ;
unsigned int use = 0 ;
unsigned int count = 0 ;
chunk *chunks[MAX_NUMBER_OF_CHUNKS];

/*files elements*/
file files[MAX_NUMBER_OF_FILES];
int NumberOFFiles=0;
int currentFile=0;
unsigned int NextChunkNumber=0; 
char allIsDone=0;

/*To map Files Into Memory For Fast&Esay Reading*/
void mapFilesToMemory(int argc, char *argv[])
{
    int i=1;
    while(i<argc)
    {
        int fd = open(argv[i], O_RDONLY);
        if (fd == -1) 
        {
            fprintf(stdout,"%s","wzip: cannot open file\n");
            exit(EXIT_FAILURE);
        }

        struct stat sb;
        if (fstat(fd, &sb) == -1) 
        {
            fprintf(stdout,"%s","wzip: cannot get stat\n");
            close(fd);
            exit(EXIT_FAILURE);
        }


        char *mapped = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (mapped == MAP_FAILED) 
        {
           fprintf(stdout,"%s","Error mmapping the file");
           close(fd);
           exit(EXIT_FAILURE);
        }

        files[i-1].fileId=i-1;
        files[i-1].fileSize=sb.st_size;
        files[i-1].fileSizeConst=sb.st_size;;
        files[i-1].fileInMemory=mapped;
        files[i-1].fileCursor=mapped;
        i++;
        NumberOFFiles++;
        close(fd);
    }

}

void enqueue(chunk *c)
{
  chunks[fill] = c ;
  fill = (fill + 1) % MAX_NUMBER_OF_CHUNKS ;
  count++ ;
}
chunk *dequeue(void)
{
  chunk *c = chunks[use];
  use = (use + 1) % MAX_NUMBER_OF_CHUNKS ;
  count-- ;
  return c;
}
void* produce(void* arg)
{
    while(currentFile<NumberOFFiles)
    {
    chunk *c=malloc(sizeof(chunk));
    //can be in one chunk
    if(files[currentFile].fileSize<=DEFAULT_CHUNCK_SIZE)
    {
        c->lastInFile=0;
        c->chunkIsLast=0;
        c->chunkStarting=files[currentFile].fileCursor;
        c->chunkSize=files[currentFile].fileSize;
        c->chunkId=NextChunkNumber;
        files[currentFile].fileCursor=files[currentFile].fileCursor+files[currentFile].fileSize;
        files[currentFile].fileSize=0;
    }
    else
    {
        /*the rest of the file can't be on one segment so set the default size of the segment but with the right cut*/
        c->chunkIsLast=0;
        c->lastInFile=0;
        c->chunkStarting=files[currentFile].fileCursor;
        c->chunkId=NextChunkNumber;
        c->chunkSize=DEFAULT_CHUNCK_SIZE;
        files[currentFile].fileCursor+=DEFAULT_CHUNCK_SIZE;
        files[currentFile].fileSize-=DEFAULT_CHUNCK_SIZE;
      
    }
    if(files[currentFile].fileSize==0)
    {
        if(currentFile==(NumberOFFiles-1))
        {
            c->chunkIsLast=1;
        }
        c->lastInFile=1;
        currentFile++;
    }
    NextChunkNumber++;
    pthread_mutex_lock(&chunks_lock);
    while (count == MAX_NUMBER_OF_CHUNKS) 
    { // the queue is full 
      pthread_cond_wait(&empty_chunks_cv, &chunks_lock); // sleep 
    }
    enqueue(c);
    pthread_cond_signal(&fill_chunks_cv); //wake a thread to consume
    pthread_mutex_unlock(&chunks_lock);
    }
    return NULL;
}

void allocateForEncodedChunk(chunk*c)
{
    size_t s =sizeof(int)+(sizeof(char)*c->chunkSize);
    c->chunkEncodedInMemory=malloc(s);
    if(c->chunkEncodedInMemory==NULL)
    {
        exit(EXIT_FAILURE);
    }
    c->chunkCursor=c->chunkEncodedInMemory;
    c->chunkfree=c->chunkEncodedInMemory;
}

void encodeRLE(chunk*c)
{
    int current=0,counter=1;
    char*cursor=c->chunkStarting;
    allocateForEncodedChunk(c);
    while(current<(c->chunkSize))
    {
        if(cursor[current]==cursor[current+1])
        {
            counter++;
            
            if(current==(c->chunkSize-1))
            {
                //write to zipped buffer
                memcpy(c->chunkCursor, &counter, sizeof(int));
                c->chunkCursor +=sizeof(int);
                memcpy(c->chunkCursor, &cursor[current], sizeof(char));
                c->chunkCursor +=sizeof(char);
                counter=1;
            }
        }
        else
        {
            //write to zipped buffer
            memcpy(c->chunkCursor, &counter, sizeof(int));
            c->chunkCursor +=sizeof(int);
            memcpy(c->chunkCursor, &cursor[current], sizeof(char));
            c->chunkCursor +=sizeof(char);
            counter=1;
        }
        
        current++;
    }

zippedData[c->chunkId]=c;


}


void* consume(void* arg)
{
    while (1) 
    {
        pthread_mutex_lock(&chunks_lock);
        while(count==0)
        {
            if(allIsDone==1)
            {
                break;
            }
            pthread_cond_wait(&fill_chunks_cv,&chunks_lock);
        }
        if(allIsDone==1)
        {
            pthread_mutex_unlock(&chunks_lock);
            break;

        }
        chunk * c=dequeue();
        if(c->chunkIsLast)
        {
            allIsDone=1;
            pthread_cond_broadcast(&fill_chunks_cv);//wake all threads to terminate

        }
        pthread_cond_signal(&empty_chunks_cv); //wake a thread to produce 
        pthread_mutex_unlock(&chunks_lock);
        encodeRLE(c);
    
    }
    return NULL;
}

/*remember to clean the heap from chunks and the encoded chunks */
int main(int argc, char *argv[])
{
    if (argc < 2) 
    { 
        fprintf(stdout,"%s","wzip: file1 [file2 ...]\n");
        exit(EXIT_FAILURE);
    }
    mapFilesToMemory(argc,argv);
  

    int numberOFThreads = get_nprocs();/*number of threads*/
    pthread_t producerthread;
    pthread_t consumersthreads[numberOFThreads-1];
    int consumersthreadsIds[numberOFThreads-1];
    int rc = pthread_create(&producerthread,NULL,produce,NULL);
    assert(rc==0);
    for(int i=0;i<numberOFThreads-1;i++)
    {
        consumersthreadsIds[i]=i;
        int rc = pthread_create(&consumersthreads[i],NULL,consume,&consumersthreadsIds[i]);
        assert(rc==0);
    }

     /*waitting for the threads*/
    pthread_join(producerthread, NULL);
     for (int i = 0; i<numberOFThreads-1; i++) 
     {
        pthread_join(consumersthreads[i], NULL);
     }


 int flag=0,sum=0,i,first=0;
     for(i=0;i<NextChunkNumber;i++)
     {

        if((i+1)!=NextChunkNumber)
        {
           
            if(*(zippedData[i]->chunkCursor-1)==*(zippedData[i+1]->chunkEncodedInMemory+4))
            {
                
                if(flag==0)
                {
                    first=i;
                    sum=(*(int*)((char*)(zippedData[i]->chunkCursor)-5));
                }
                sum+=(*(int*)(zippedData[i+1]->chunkEncodedInMemory));
               // printf("%d\n",sum);
                zippedData[i+1]->chunkEncodedInMemory+=5;
                flag=1;
            }
            else
            {
                if(flag==1)
                {
                    (*(int*)((char*)(zippedData[first]->chunkCursor)-5))=sum;
                    
                }
                sum=0;
                flag=0;
            }

        }

     }

    if(flag==1)
    {
        (*(int*)((char*)(zippedData[first]->chunkCursor)-5))=sum;
                    
    }



     /*write the zipped data to stdout & free the allocated memory*/
    for(int i=0;i<NextChunkNumber;i++)
     {
        write(STDOUT_FILENO, zippedData[i]->chunkEncodedInMemory, zippedData[i]->chunkCursor - zippedData[i]->chunkEncodedInMemory);
        free(zippedData[i]->chunkfree);
        zippedData[i]->chunkfree=NULL;
        free(zippedData[i]);
        zippedData[i]=NULL;
     }




    /*unmap the files from the memory*/
    for(int i=0;i<NumberOFFiles;i++)
    {
        if (munmap(files[i].fileInMemory, files[i].fileSizeConst) == -1) 
         {
            fprintf(stdout,"%s","Error unmapping the memory");
            exit(EXIT_FAILURE);
         }

    }
    
}
