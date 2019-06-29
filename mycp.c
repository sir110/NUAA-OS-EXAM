#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
   char buf=NULL;
   int fd = open(argv[1], O_RDONLY);
   int fd2 = open(argv[2], O_WRONLY | O_TRUNC);
   int count = read(fd, &buf, sizeof(buf));
   while(count != 0)
   {
      write(fd2, &buf, sizeof(buf));
      count=read(fd, &buf, sizeof(buf));
   }
   close(fd);
   close(fd2);
   return 0;
}
