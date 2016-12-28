#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "demo/learn.h"//自己写的一些东西
#define ISspace(x) isspace((int)(x))

#define SERVER_STRING "Server: My Love from Cloud\r\n"
void * accept_request(void *thread_arg);
int startup(u_short *port);
void error_die(const char *sc);
//主函数入口,编译时

//gcc -W -Wall -pthread -o httpd httpd.c
//参考开源tinyhttpd
int main(int argc, char *argv[]) {
    int server_sock = -1;
    if (argv[1] == NULL)
        argv[1] = "80";
    u_short port = atoi(argv[1]);  //atoi 将字符串转换为整形
    int client_sock = -1;
    struct sockaddr_in client_name;
    int client_name_len = sizeof (client_name);
    pthread_t newthread;
    //建立socket，监听端口
    server_sock = startup(&port);
    printf("httpd running on port %d\n", port);
    //获取程序目录  
    char dirPath[128];
    getcwd(dirPath, 128);
    printf("current program Path: %s\n",dirPath);
    while (1) {
        client_sock = accept(server_sock,
                (struct sockaddr *) &client_name,
                &client_name_len);
        if (client_sock == -1)
            error_die("accept");
       // PrintSocketAddress((struct sockaddr*) &client_name, stdout);
       // accept_request(client_sock); 
          //printf("client_sock:%d\n",client_sock);
        //子线程处理http 请求
        if (pthread_create(&newthread, NULL, accept_request, (void*) client_sock) != 0)
           perror("pthread_create");
    }

    close(server_sock);

    return (0);
}

void * accept_request(void *thread_arg) {
    //子线程处理http请求


    int client;
	int n;
	int box[1024];
	char buf[1024*2048];
    client = (int) thread_arg;
	List *head;
	char *getpath;
	getpath=getrunpath();
	head=getdirlist(getpath);//得到保存文件名的指针
    memset(buf,'\0',sizeof(buf));//全部置空。
	if(n = recv(client, box, sizeof(box), 0)>0){ //接收客户机数据
	strcat(buf,"HTTP/1.0 200 OK\r\n");
    //strcpy(buf, "HTTP/1.0 200 OK\r\n");
    //send(client, buf, strlen(buf), 0);
    strcat(buf, SERVER_STRING);
	strcat(buf,"Content-Length:");
	strcat(buf,"10240");
	strcat(buf,"\r\n");
	strcat(buf, "Content-Type: text/html\r\n");
	strcat(buf,"\r\n <html><head>");
	strcat(buf,"<title>Index of ");
	strcat(buf,getpath);
	strcat(buf,"</title></head><body>");
	strcat(buf,"<h1>Index of ");
	strcat(buf,getpath);
	strcat(buf,"</h1><table>");
	while(head->next!=NULL){
		head=head->next;
		if(strcmp(head->filename,".")==0) continue;
		if(strcmp(head->filename,"..")==0) continue;
		
		strcat(buf,"<tr><td><a href='");
		strcat(buf,head->filename);
		strcat(buf,"'>");
		strcat(buf,head->filename);
		strcat(buf,"</a></tr></td>");
		
	}
	strcat(buf,"</table>tristan's server (Ubuntu) Server 2016</address></body></html>\r\n");
	write(client,buf,1024);
	}
    close(client);
    return (NULL);
}
void error_die(const char *sc) {
    perror(sc);
    exit(1);
}
int startup(u_short *port) {
    int httpd = 0;
    struct sockaddr_in name;

    httpd = socket(PF_INET, SOCK_STREAM, 0);
    if (httpd == -1)
        error_die("socket");
    memset(&name, 0, sizeof (name));
    name.sin_family = AF_INET;
    name.sin_port = htons(*port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(httpd, (struct sockaddr *) &name, sizeof (name)) < 0)
        error_die("bind");
    if (*port == 0) /* if dynamically allocating a port */ {
        int namelen = sizeof (name);
        if (getsockname(httpd, (struct sockaddr *) &name, &namelen) == -1)
            error_die("getsockname");
        *port = ntohs(name.sin_port);
    }
    if (listen(httpd, 5) < 0)
        error_die("listen");
    return (httpd);
}
