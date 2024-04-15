#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
//macros
#define MAX_ARGS            20
#define MAX_PATH_LENGTH     30
#define MAX_PATHS           10
#define MAX_TOKENS          100
#define MAX_COMMANDS        100
#define SUCESS              0
#define FAIL                1

//global variables
char * commands[MAX_COMMANDS][MAX_TOKENS];
int commandTokensCounter[MAX_COMMANDS]={0};
int commandsNumber=0;
char *PATH[MAX_PATHS]={"/bin","/usr/bin"};
int NumberOfPath=2;


void error()
{
	char error_message[40] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
}

void input_Normalize(char*des,char*src)
{
    int i=0,j=0;
    while(src[i])
    {
        if(src[i]=='&'||src[i]=='|'||src[i]=='>')
        {
            des[j]=' ';
            des[j+1]=src[i];
            des[j+2]=' ';
            i++;
            j+=3;
            continue;
        }
        des[j]=src[i];
        j++;
        i++;
    }
    des[j]='\0';
}

void tokenize(char *input)
{
    char *nomalizedInput=malloc(strlen(input)*2);
    input_Normalize(nomalizedInput,input);
    char *token=NULL;
    /*if there is delimeter*/
    token=strtok(nomalizedInput, " \n");
    while(token!=NULL)
    { 
        /*remember to free*/
        if(strcmp(token, "&")==0||strcmp(token, "|")==0)
        {
           
           token=strtok(NULL," \n");
            if(token==NULL)
            {
                break;

            }
            commandsNumber++;
        }
       
        commands[commandsNumber][((commandTokensCounter[commandsNumber]))]=malloc(MAX_INPUT*sizeof(char));
        strcpy( commands[commandsNumber][((commandTokensCounter[commandsNumber]))], token);
        commandTokensCounter[commandsNumber]++;
        token=strtok(NULL," \n");
    }
    free(nomalizedInput);
}

void getExcutablePath(char*command,int commandIndex,int pathIndex)
{
    free(commands[commandIndex][0]);
    commands[commandIndex][0]=malloc(MAX_INPUT);
    strcpy( commands[commandIndex][0],PATH[pathIndex]);
    strcat( commands[commandIndex][0],"/");
    strcat( commands[commandIndex][0],command);  

}


void commandDestructor()
{
     
    for(int i=0;i<=commandsNumber;i++)
    {
        for(int j=0;j<commandTokensCounter[i];j++)
        {
            free(commands[i][j]);
        }
        commandTokensCounter[i]=0;
    }
    commandsNumber=0;

}

void excuteCommand()
{
    int original_stdout = dup(STDOUT_FILENO);
    int original_stderr = dup(STDERR_FILENO);
    char  commandName[50];
    char*argv[MAX_ARGS];
    pid_t pids[commandsNumber+1];
    for(int i=0;i<=commandsNumber;i++)
    { 
        strcpy(commandName, commands[i][0]);
        //built in command
        if(strcmp(commandName,"exit")==0)
        {
            if(commandTokensCounter[i]!=1)
            {
                //error
                error();
            }
            else 
            {
                for(int j=2;j<NumberOfPath;j++)
                {
                    free(PATH[j]);
                }
                commandDestructor();
                exit(SUCESS);
            
            } 

        }
        else if (strcmp(commandName,"cd")==0)
        {
            if(chdir(commands[i][1])!=0)
            {
                //error
                error();
            }

        }
        else if(strcmp(commandName,"path")==0)
        {
            int j;
            int counter=0;
            for(j =1;j<commandTokensCounter[i];j++)
            {
                char *result = realpath(commands[i][j], NULL);
                if(result !=NULL)
                {
                    counter++;
                    PATH[j-1]=malloc(MAX_PATH_LENGTH);
                    strcpy(PATH[j-1], commands[i][j]);
                    NumberOfPath=counter;
                }
                else
                {
                    error();
                }                
            }
            if(j==1)/*no argument*/
            {
                strcpy(PATH[0],"");
            }
        }
        //not built in
        else
        {
            //you need to check the paths
            if(access(commands[i][0], X_OK)!=0)
            {

                int j;
                for(j=0;j<NumberOfPath;j++)
                {
                    getExcutablePath(commandName,i,j);
                    //printf("%s\n",commands[i][0]);
                    if(access(commands[i][0], X_OK)==0)
                    {

                        break;
                    }
                }
                if(j==NumberOfPath)
                {   
                    error();
                    //error command not found
                    break;
                }
                for(j=0;j<commandTokensCounter[i];j++)
                {
                    if(strcmp(commands[i][j],">")==0)
                    {
                        if(j!=0&&(j+2)==commandTokensCounter[i])
                        {
                            
                            //make redirection
                            int fd = open(commands[i][j+1], O_WRONLY | O_CREAT|O_TRUNC , 0777);
                            dup2(fd,STDOUT_FILENO);
                            dup2(fd,STDERR_FILENO);
                            free(commands[i][j]);
                            free(commands[i][j+1]);
                            commands[i][j]=NULL;
                            commands[i][j+1]=NULL;
                            commandTokensCounter[i]-=2;
                            close(fd);
                            break;

                        }
                        else 
                        {
                            error();
                            exit(SUCESS);
                            //error
                        }

                    }
                    else if(strcmp(commands[i][j],">>")==0)
                    {
                        error();
                        exit(SUCESS);
                        //error
                    }
                } 
            
            }
            pids[i]=fork();
             if (pids[i] == -1) 
             {
                // Error occurred
                perror("fork");
             }

            if (pids[i] == 0) 
            {
                int j=0;
                //check for redirection
               
                // Child process
                for(j=0;j<commandTokensCounter[i];j++)
                {
                    argv[j]=commands[i][j];
                }
                argv[0]=commandName;
                argv[j]=NULL;
                execv(commands[i][0],argv);
            } 
            else 
            {
            
              
              
            }
            
        }
    }
    for(int i=0;i<=commandsNumber;i++)
            {
                int state;
                waitpid(pids[i], &state, 0);
                // Parent process
                dup2(original_stdout, STDOUT_FILENO);
                dup2(original_stderr, STDERR_FILENO);
            }
  
   
}



void shell(FILE *fp)
{
    size_t size=0;
    char *command=NULL;
    while(1)
    {
        if(fp==stdin)
        {
            //interactive mode
            printf("wish> ");
        }
        
        if(getline(&command, &size, fp)==EOF)
        {
            break;
        }
        if(strcmp(command, "&\n")==0||strcmp(command, "|\n")==0||strcmp(command, ">\n")==0)
        {
            continue;
        }
        tokenize(command);
        if(commandTokensCounter[0])
        {
          excuteCommand();
          commandDestructor();
        }
        size=0;
        free(command);
        command=NULL;
    }
}

int main(int argc,char *argv[])
{
    if(argc==1)/*interactive mode*/
    {
        shell(stdin);
    }
    else if(argc==2)/*batch mode*/
    {
        FILE *fp = fopen(argv[1],"r");
        if(fp != NULL)
        {
            shell(fp);
        }
        else
        {
            error();
            exit(FAIL);
        }
    }
    else 
    {
        error();
        exit(FAIL);
    
    }
    return 0;
}