#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#define MAX_LINE		80 /* 80 chars per line, per command */

//char history[1024];
int flag_and;

int getcommand(char* args[MAX_LINE / 2 + 1], char str[1024]);
int execution(char* args[MAX_LINE / 2 + 1] );
char* history=NULL;

int main(void)
{
	char* args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
    	int should_run = 1;
        printf("this is my shell ,print |quit| to exit the program.\n");
        while (should_run)
	{
        	printf("osh>");
            char str[1024];
            fgets(str, 1024, stdin);	//get input

            int flag=getcommand(args, str);	//test whether wait the child process

			char *s = "quit\0";
            
            if(strcmp(args[0], s)==0){
				printf("quit command");
                break;
            }
               
				
            pid_t pid = fork();
					if (pid == 0)
				{
                    int test = execvp(args[0], args);
                    if(test==-1){
                        printf("wrong command\n");  // have history command
                    }
                        
                    fflush(stdout);
                    exit(0);
                }

					else  if(!flag)
						wait(0);
            
	}
	
	return 0;
}

int getcommand(char* args[MAX_LINE / 2 + 1], char str[1024])		
{
    int size = 0;
    int s=strlen (str);
	int j=0;

	int m=0;
	while (str[m]==' ')m++;
	
	//test !! command
	if(str[m]=='!'&&str[m+1]=='!'){
		if(history!=NULL)
            strcpy(str, history);
		else
            printf("no history command");
        return 0;
    }

 	s=strlen (str);
	m=0;
	while (str[m]==' ')m++;
	
	flag_and = 0;
	//if(args[0]==NULL)
    args[0]=(char*) malloc(sizeof(char)*30);

    for(int i=m;i<s;i++)
	{
		if(str[i]==' ')	{	
		while(str[i]==' ')i++;
		if(args[size]!=NULL)
		args[size++][j]='\0';	
		else
            printf("NULL array space\n");
        j=0;
		if(str[i]!='\n')
		if(args[size]==NULL)
		args[size]=(char*) malloc(sizeof(char)*30);
		}

		if(str[i]=='\n'){
			args[++size]=NULL;
			if(history==NULL)
                history = (char*) malloc(1024 * sizeof(char));
            strcpy(history, str);
            break;
        }

		if(str[i]!=' ')	
		{
			if(str[i]=='&')
            {
				flag_and = 1;
				args[++size]=NULL;
               	if(history==NULL)
                history = (char*) malloc(1024 * sizeof(char));
            	strcpy(history, str);
                break;
			}
			if(args[size]!=NULL)
            args[size][j]=str[i];//normal input
			else
                printf("NULL array word \n");
            j++;
		}

	}

    return flag_and;
} 

