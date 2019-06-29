#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
int parseline(char *buf, char **argv);
int buildinCommand(char **argv);

void eval(char *cmd) 
{
    char *argv[BUFFER_SIZE] = {NULL};
    char buf[BUFFER_SIZE] = {0};
    int j;
    for (j = 0; j < BUFFER_SIZE; j++) 
        argv[j] = NULL;
    strcpy(buf, cmd);
    parseline(buf, argv);
    if (argv[0] == NULL) 
    	return ;
    if (buildinCommand(argv)) 
    	return;
    pid_t pid = fork();
    if (pid == 0) 
    {
        // > 输出重定向
        int i = 0;
        char** tmpcmd = (char**) malloc(sizeof(char*) * 20); 
        int tcmdp = 0;  // tmp cmd pointer
        int fd[512] = {0};
        int recur = -1;
        int sfd = dup(STDOUT_FILENO);
        // < 输入重定向
        int openfd[512] = {0};
        int open_recur = -1;
        // | 管道
        int forkfd[2];
        // 实现命令ll
        if(strcmp(argv[0], "ll") == 0) 
        {
            tmpcmd[0] = "ls";
            tmpcmd[1] = "-l";
            i++;
            tcmdp += 2;
        }

        while(argv[i] != NULL) 
        {
            // printf("[eval] argv[%d] = %s\n", i, argv[i]);
            if(strcmp(argv[i], ">") == 0) 
            {
                if(strcmp(argv[i+1], ">") == 0) 
                {
                    printf("syntax error near unexpected token '>'\n");
                    return;
                }
                // 打开文件
                recur++;
                fd[recur] = open(argv[i+1], O_CREAT|O_RDWR|O_TRUNC, 0755);
                // i加2指向下一个命令
                i+=2;
            }
            else if(strcmp(argv[i], "<") == 0) 
            {
                if(strcmp(argv[i+1], "<") == 0) 
                {
                    printf("syntax error near unexpected token '<'\n");
                    return;
                }
                // 打开文件
                open_recur++;
                openfd[open_recur] = open(argv[i+1], O_RDONLY);
                dup2(openfd[open_recur], 0); // 输入重定向
                // i加2指向下一个命令
                i+=2;
            }
            else if(strcmp(argv[i], "|") == 0) 
            {
                pipe(forkfd); // 管道
                int pipepid = fork();

                if(pipepid == 0) 
                {
                    tmpcmd[tcmdp] = NULL;
                    dup2(forkfd[1], 1); 
                    close(forkfd[0]);
                    close(forkfd[1]);
                    execvp(tmpcmd[0], tmpcmd);
                    // printf("[shell] pipe closed\n");
                    exit(0);
                }
                else 
                {
                    wait(&pipepid);
                    // printf("[shell] pipe fucked!\n");
                    dup2(forkfd[0], 0); 
                    close(forkfd[0]);
                    close(forkfd[1]);
                    i++;
                    tcmdp = 0;
                    continue;
                }
            }
            // 最后的命令
            else 
            {
                tmpcmd[tcmdp] = (char*)malloc(sizeof(char) * 512);
                strcpy(tmpcmd[tcmdp], argv[i]);
                i++;
                tcmdp++;
            }
        }
        tmpcmd[tcmdp] = NULL;
        if(recur != -1) 
        {
            dup2(fd[recur], 1); // 重定向输出
        }
        if(execvp(tmpcmd[0], tmpcmd) < 0) 
        {
            dup2(sfd, STDOUT_FILENO);
            printf("%s: command not found\n", argv[0]);
            return;
        }
        while(recur > 0) 
        {
            close(fd[recur]);
            recur--;
        }
        while(open_recur > 0) 
        {
            close(openfd[open_recur]);
            open_recur--;
        }
        close(fd[0]);
        close(openfd[0]);
        dup2(sfd, STDOUT_FILENO);   // 恢复输出
    }
    else 
        wait(NULL);
}

int parseline(char *buf, char **argv)
{ 
    if (strcmp(buf, "\n") == 0) 
    {
        return 0;
    }
    char *p = NULL;
    int argc = 0;
    p = strtok(buf, " \n");  // 分解字符串为一组字符串
    while (p != NULL) 
    {
        argv[argc++] = p;
        p = strtok(NULL, " \n"); // 再次调用要把s设成NULL
    }
    argv[argc] = NULL;
    return 0;
}

int buildinCommand(char **argv)
{
    if(strcmp(argv[0],"exit") == 0)
    {
        exit(0);
    }
    if(strcmp(argv[0],"cd") == 0)
    {
        if(chdir(argv[1]))
            printf("cd: no such file or directory: %s\n",argv[1]);
        return 1;
    }
    if(strcmp(argv[0],"pwd") == 0)
    {
        char buf[BUFFER_SIZE];
        printf("%s\n",getcwd(buf,sizeof(buf)));
        return 1;
    }
    return 0;
}


int main (int argc, char *argv[]) 
{
	// buf用于缓存输入的命令
    char buf[BUFFER_SIZE] = {0};
    int n;
    while(1) 
    {
        printf("> ");
        fflush(stdout);//清除读写缓冲区
        int i;
        for (i = 0; i < BUFFER_SIZE; i++) 
        {
            buf[i] = 0;
        }
        // 直接使用文件描述符0读取输入的字符
        if ((n = read(0, buf, BUFFER_SIZE)) < 0) 
        {
            printf("wrong in input .\n");
        }
        // 命令运行
        eval(buf);
    }
    return 0;
}