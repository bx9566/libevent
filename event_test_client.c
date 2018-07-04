/*************************************************************************
    > File Name: event_test_client.c
    > Author: bao
    > Mail: 1356632086@qq.com 
    > Created Time: 2018年07月02日 星期一 21时05分47秒
 ************************************************************************/

#include<stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <event2/event.h>
int main(int argc,char* argv[])
{
const char *str="hello event";
int fd=0;
fd=open("event.fifo",O_RDWR);
while(1)
{
	write(fd,str,strlen(str));
	sleep(1);
}
close(fd);
return 0;
}

