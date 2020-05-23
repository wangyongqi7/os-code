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

int bool_in,bool_out,bool_and;
char* history=NULL;
char* in_file;
char* out_file;
int getcommand(char* argv[MAX_LINE / 2 + 1], char str[1024]);
int execution(char* argv[MAX_LINE / 2 + 1] ,char *str);


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
            //printf("pout_get_commanf");
            
            int x=getcommand(args,str);  // test whether wait the child process
            //return 0;
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
    bool_and =bool_in=bool_out= 0;

    int m = 0;
    

    //test !! command
	if(str[m]=='!'&&str[m+1]=='!'){
		if(history!=NULL)
		{
			strcpy(str, history);
			s=strlen (str);
			m=0;
			while (str[m]==' ')m++;
		}
            
		if(history==NULL)
		{
 			printf("no history command");
            return 0;
        }
           
    }
    
    
    history = (char*)malloc(sizeof(char) * 1024);
    strcpy(history, str);
    int status=0;
    int size = 0;
    
    for(int i =0; str[i] != 0; ++i){
        if(str[i]=='&'||str[i]=='<'||str[i]=='>')
        {
            argv[size++] = NULL;
            if(str[i]=='&')
                bool_and = size;
            if(str[i]=='<')
                {
                    bool_in = i+1;
                while (str[bool_in]==' ')
                    bool_in++;
                }
            if(str[i]=='>')
            {
                bool_out = i+1;
                while (str[bool_out]==' ')
                    bool_out++;
            }
                
            str[i] = 0;
        }
        if(status ==0 && !isspace(str[i])){
            argv[size++] = str +i;
            status = 1;
        }
        else if(isspace(str[i])){
            status = 0;
            str[i] = 0;
        }
    }
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
                
                in_file = (char*)malloc(sizeof(char) * 30);
                for (int i = bool_in; history[i] != '\n'&&history[i]!='\0';i++)
                {
                    in_file[i - bool_in] = history[i];
                }
                
               int fd =open(in_file, O_RDWR|O_CREAT|O_TRUNC, 0664);
               
                if(fd == -1)perror("open"),exit( 1 );
                //重定向操作
                dup2(fd,STDIN_FILENO); //dup2(oldfd, newfd);
                close(fd);
            }
                
            if(bool_out>0)
			{
                out_file = (char*)malloc(sizeof(char) * 30);
                for (int i = bool_out; history[i] != '\n'&&history[i]!='\0';i++)
                {
                    out_file[i - bool_out] = history[i];
                }
                
                int fd =open(out_file, O_RDWR|O_CREAT|O_TRUNC, 0664);
               
                if(fd == -1)perror("open"),exit( 1 );
                //重定向操作
                dup2(fd, STDOUT_FILENO); //dup2(oldfd, newfd);
                close(fd);
            }

			int test = execvp(argv[0], argv);
            if (test == -1)
            {
                printf("wrong command\n");  // have history command
            }
			 perror("fork"); //执行到此，说明execvp未执行成功，fork失败
        	exit(1);
        }

		else  if(bool_and==0)
            waitpid(pid, 0, 0);
        return 0;
}
