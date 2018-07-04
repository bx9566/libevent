/*************************************************************************
    > File Name: event_test_service.c
    > Author: bao
    > Mail: 1356632086@qq.com 
    > Created Time: 2018年07月02日 星期一 21时05分33秒
 ************************************************************************/

#include<stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <event2/event.h>
void callback_func(evutil_socket_t fd, short event, void *arg)
{
	char buf[256]={0};
	int len=0;
	struct event_base *base=(struct event_base *)arg;
	printf("fd=%d,event=%d",fd,event);
	len=read(fd,buf,sizeof(buf));
	if(len==-1)
	{
	perror("read");
	return;
	}
	else if(len ==0)
	{
	perror("remote close fd");
	return;
	}
	else
	{
	buf[len]='\0';
	printf("read buf=[%s]\n",buf);
	FILE*fp=fopen("event_test.txt","a");
	if(fp==NULL)
	{
	perror("fopen error");
	exit(1);
	}
	event_base_dump_events(base,fp);
	fclose(fp);
	}
       return;
}
int main(int argc,char* argv[])
{
struct event_base *base=NULL;
struct event *evfifo=NULL;
const char *fifo="event.fifo";
int fd;
unlink(fifo);
if(mkfifo(fifo,0644)==-1){
	perror("mkfifo");
	exit(1);
	}
fd=open(fifo,O_RDONLY);
if(fd==-1){
	perror("open socket error");
	exit(1);
	}
base=event_base_new();
evfifo=event_new(base,fd,EV_READ|EV_PERSIST,callback_func,base);
event_add(evfifo,NULL);
event_base_dispatch(base);
event_free(evfifo);
event_base_free(base);
return 0;
}

