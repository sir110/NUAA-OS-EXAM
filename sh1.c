#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>

#define BUFFER_SIZE 1024
int parseline(char *buf, char **argv);
int buildinCommand(char **argv);

void eval(char *cmd) 
{
    char *argv[BUFFER_SIZE] = {NULL};
    char buf[BUFFER_SIZE] = {0};
    int j;
    for (j = 0; j < BUFFER_SIZE; j++) 
    {
        argv[j] = NULL;
    }
    strcpy(buf, cmd);
    parseline(buf, argv);
    if (argv[0] == NULL) 
    {
        return ;
    }

    if (buildinCommand(argv)) return;
    
    pid_t pid = fork();
    if (pid == 0) 
    {
        if (execvp(argv[0], argv) < 0) 
        {
            // 运行失败
            printf("%s: command not found...\n", argv[0]);
            exit(0);
        }
    } 
    else 
    {
        wait(NULL);
    }
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