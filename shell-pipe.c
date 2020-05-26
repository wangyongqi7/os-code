#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#define MAX_LINE		80 /* 80 chars per line, per command */

int bool_in,bool_out,bool_and,bool_pipe;
char history[1024]={};
char* in_file;
char* out_file;
char* file;
int getcommand(char* argv[MAX_LINE / 2 + 1], char str[1024]);
int execution(char* argv[MAX_LINE / 2 + 1] ,char *str);
char* child_args[MAX_LINE/2 + 1];

int main(void)
{
	char* args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
    	int should_run = 1;
        printf("this is my shell ,print |quit| to exit the program.\n");

        int flag;
        while (should_run)
        {
        	printf("osh>");
            char str[1024]={};
            fgets(str, 1024, stdin);	//get input

            
            int x=getcommand(args,str);  // test whether wait the child process

            if(str[0]=='\n'||str[0]=='\0')
                continue;
                char s[] ="quit";
                if(strcmp(args[0], s)==0){
                    printf("quit command\n");
                    break;
                }

                execution(args, str);
            
            
    }
	
	return 0;
}

int getcommand(char* argv[MAX_LINE / 2 + 1], char str[1024])		
{
    if(str[0]=='\n')
        return 0;
    int s=strlen (str);
    bool_and =bool_in=bool_out=bool_pipe= 0;

    int m = 0;
    

    //test !! command
	if(str[m]=='!'&&str[m+1]=='!'){


        if(history[0]!='\0'&&history[0]!='\n')
		{
			strcpy(str, history);
            printf("%s", history);
            s=strlen (str);
			m=0;
			while (str[m]==' ')m++;
		}
            
		else
		{
 			printf("no history command");
            return 0;
        }
           
    }
    
    
    //history = (char*)malloc(sizeof(char) * 1024);
    strcpy(history, str);
    int status=0;
    int size = 0;
    int size2 = 0;
    for(int i =0; str[i] != 0; ++i){
        if(str[i]=='&'||str[i]=='<'||str[i]=='>'||str[i]=='|')
        {
            argv[size++] = NULL;
            //size2 = size;   //second command
            if(str[i]=='&')
                bool_and = size;
            if(str[i]=='<')
                {
                    bool_in = i+1;
                while (str[bool_in]==' ')
                    bool_in++;
                in_file = str + bool_in;
                }
            if(str[i]=='>')
            {
                bool_out = i+1;
                while (str[bool_out]==' ')
                    bool_out++;
                out_file = str + bool_out;
                file = str + bool_out;
            }
            if (str[i]=='|')
            {
                bool_pipe = i;
                while (str[bool_pipe]==' ')
                    bool_pipe++;
                //child_args[0] = str + bool_pipe;
                
                i++; 
            }
            //i++;

            //str[i] = 0;
        }
        if(status ==0 && !isspace(str[i])){
            if(bool_pipe>0)
            {
                argv[size] = NULL;
                /*while(str[i]==' '){
                    str[i] = 0;
                    i++;
                }*/
               // printf("the child is at i:%d  size: %d", i, size2);

                child_args[size2] = str + i;
            }
               
            else
            {
                 argv[size++] = str + i;
            }

            status = 1;
        }
        else if(isspace(str[i])){
            status = 0;
            str[i] = 0;
        }
    }
    if(bool_pipe>0)
    child_args[++size2] = NULL;
    else
        argv[size] = NULL;


    return 0;
} 


int execution(char* argv[MAX_LINE / 2 + 1] ,char *str){
    int j;

    for (int i = 0; i < strlen(str);i++)
		if(str[i]!=' '&&str[i]!='\0')
            j++;
	if(j==1)
    	return 0;
	
			
	fflush(stdout);

    pid_t pid = fork();

        if (pid == 0)
        {
            if(bool_in>0)					//redirect
			{
               int fd =open(in_file, O_RDWR|O_CREAT|O_TRUNC, 0664);

                if(fd == -1)perror("open"),exit( 1 );
                //重定向操作
                dup2(fd,STDIN_FILENO); //dup2(oldfd, newfd);
                close(fd);
            }
                
            if(bool_out>0)
			{
                int fd =open(out_file, O_RDWR|O_CREAT|O_TRUNC, 0664);

                if(fd == -1)perror("open"),exit( 1 );
                //重定向操作
                dup2(fd, STDOUT_FILENO); //dup2(oldfd, newfd);
                close(fd);
            }

            if(bool_pipe>0)
            {
                int fd[2];
                int rt=pipe(fd);
                if(rt<0)
                    printf("fail to create pipe \n");

                pid_t pid2 = fork();
                if(pid2==-1)
                {
                    printf("fail to create child process\n ");
                }
                else if(pid2==0)         //child
                {
                    close(fd[0]);
                    dup2(fd[1], STDOUT_FILENO);//outstream

                   int test = execvp(argv[0], argv);
                   if(test==-1)
                       printf("worng command\n");
                    exit(1);
                }
                else
                  {
                      close(fd[1]);
                      dup2(fd[0], STDIN_FILENO);
                      execvp(child_args[0], child_args);
                  } 
            }

			else
            {
            int test = execvp(argv[0], argv);
            if (test == -1)
            {
                printf("wrong command\n");  // have history command
            }
			 //perror("fork"); //执行到此，说明execvp未执行成功，fork失败
            }
               
        	exit(1);
        }

		else  if(bool_and==0)
            waitpid(pid, 0, 0);
        return 0;
}