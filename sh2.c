#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
void exe(char *buf, char *argv[])
{
    pid_t pid = fork();
    if(pid == 0)
    {
        int i;
        for(i = 0; argv[i] != NULL; i++)
        {
            if(strcmp(argv[i], ">") == 0)
            {
                if(argv[i+1] == NULL)
                    perror("command '>' no option");
                argv[i] = NULL;
                int fd = open(argv[i+1], O_RDWR | O_CREAT | O_TRUNC, 0664);
                if(fd == -1)
                    perror("open error");
                dup2(fd, 1);
                close(fd);
            }
        }
        execvp(buf, argv);
        perror("fork");
        exit(1);
    }
    wait(NULL);
}

void parse(char *buf)
{
    char *argv[8] = {};
    int argc = 0;
    int status = 0;
    int i;
    for(i = 0; buf[i] != 0; i++)
    {
        if(status == 0 && !isspace(buf[i]))
        {
            argv[argc++] = buf + i;
            status = 1;
        }
        else if(isspace(buf[i]))
        {
            status = 0;
            buf[i] = 0;
        }
    }
    argv[argc] = NULL;
    exe(buf, argv);
}

int main()
{
    char buf[1024] = {};
    while(1)
    {
        printf("> ");
        memset(buf, 0x00, sizeof(buf));
        while(scanf("%[^\n]%*c", buf) == 0)
        {
            printf("> ");
            while(getchar() != '\n');
        }
        parse(buf);
    }
    return 0;
}
