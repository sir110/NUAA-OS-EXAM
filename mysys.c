#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

void mysys(char *command)
{
    pid_t pid;
    int status;
    char a[10]="";
    int i=0;
    while(command[i]!=' ')
    {
        a[i] = command[i];
        i++;
    }
    //char *p = strtok(command, " ");
    if((pid = fork()) < 0)
    {
        status = -1;
    }
    if(pid == 0)
    {
        execl("/bin/sh", "sh", "-c", command, NULL);
    }
    wait(NULL);
}

int main()
{
    printf("--------------------------------------------------\n");
    mysys("echo HELLO WORLD");
    printf("--------------------------------------------------\n");
    mysys("ls /");
    printf("--------------------------------------------------\n");
    return 0;
}
