/*************************************************************************
    > File Name: listen_bufferevent.c
    > Author: bao
    > Mail: 1356632086@qq.com 
    > Created Time: 2018年07月03日 星期二 20时03分51秒
 ************************************************************************/

#include<stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <arpa/inet.h>
#include <errno.h>
static void echo_read_cb(struct bufferevent *bev, void *ctx)//bufferevent_data_cb readcb
{
	/* This callback is invoked when there is data to read o
	n bev. */
	struct evbuffer *input = bufferevent_get_input(bev);
	struct evbuffer *output = bufferevent_get_output(bev);
	/* Copy all the data from the input buffer to the output
	buffer. */
	evbuffer_add_buffer(output, input);
}
static void echo_event_cb(struct bufferevent *bev, short events, void *ctx)//bufferevent_event_cb eventcb
{
	if (events & BEV_EVENT_ERROR)
	perror("Error from bufferevent");
	if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
	bufferevent_free(bev);
	}
}
static void accept_conn_cb(struct evconnlistener *listener,
evutil_socket_t fd, struct sockaddr *address, int socklen,
void *ctx)
{
	/* We got a new connection! Set up a bufferevent for it.
	*/
	struct event_base *base = evconnlistener_get_base(listener);//监听事件
	struct bufferevent *bev = bufferevent_socket_new(
	base, fd, BEV_OPT_CLOSE_ON_FREE);//创建基于套接字的 bufferevent  :使用 event_*接口作为后端,通过底层流式套接字发送或者接收数据的 bufferevent
//void bufferevent_setcb(struct bufferevent *bufev,bufferevent_data_cb readcb,bufferevent_data_cb writecb,bufferevent_event_cb eventcb, void *cbarg);
	bufferevent_setcb(bev, echo_read_cb, NULL, echo_event_cb
	, NULL);
	bufferevent_enable(bev, EV_READ|EV_WRITE);//使读写生效
}
static void accept_error_cb(struct evconnlistener *listener, void *ctx)
{
	struct event_base *base = evconnlistener_get_base(listener);
	int err = EVUTIL_SOCKET_ERROR();//获取错误号
	fprintf(stderr, "Got an error %d (%s) on the listener. "
	"Shutting down.\n", err, evutil_socket_error_to_string(err));
	event_base_loopexit(base, NULL);//跳出循环
}
int main(int argc,char* argv[])
{
	struct event_base *base;
	struct evconnlistener *listener;//设置监听器
	struct sockaddr_in sin;//socket地址端口
	int port = 9876;
	if (argc > 1) {
	port = atoi(argv[1]);
	}
	if (port<=0 || port>65535) {
	puts("Invalid port");
	return 1;
	}
	base = event_base_new();//base初始化
	if (!base) {
	puts("Couldn't open event base");
	return 1;
	}
	/* Clear the sockaddr before using it, in case there are
	extra
	* platform-specific fields that can mess us up. */
	memset(&sin, 0, sizeof(sin));//地址初始化
	/* This is an INET address */
	sin.sin_family = AF_INET;
	/* Listen on 0.0.0.0 */
	sin.sin_addr.s_addr = htonl(0);
	/* Listen on the given port. */
	sin.sin_port = htons(port);
	//创建监听器事件 分配并绑定监听TCP套接字的事件对象    对应回调函数
	listener = evconnlistener_new_bind(base, accept_conn_cb,
	NULL,
	LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1,
	(struct sockaddr*)&sin, sizeof(sin));
	if (!listener) {
	perror("Couldn't create listener");
	return 1;
	}
	evconnlistener_set_error_cb(listener, accept_error_cb);//错误监听
	event_base_dispatch(base);//事件处理
	return 0;
}

