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
//#define ISspace(x) isspace((int)(x))

#define SERVER_STRING "Server: My Love from Cloud\r\n"
#define DOCPATH "/home/tristan/httpdLinuxC/docroot"
#define ISspace(x) isspace((int)(x))  //函数式宏 isspace()是库函数 检查c是否是空格
void * accept_request(void *thread_arg);
int startup(u_short *port);
void error_die(const char *sc);
void rootsend(int client);
int get_line(int sock, char *buf, int size);// 从客户输入中得到一行？
void serve_file(int client, const char *filename);//返回二进制文件流
void headers(int client, const char *filename);//文件流的头
void cat(int client, FILE *resource);//发送文件
//主函数入口,编译时

//gcc -W -Wall -pthread -o httpd myhttpd.c
//参考开源tinyhttpd
void * accept_request(void *thread_arg) {
    //子线程处理http请求


    int client;
	int n;
	char buf[1024];
	char url[255];
	char paths[255]=DOCPATH;
	int i=0,j=0;
	memset(url,'\0',sizeof(url));
	memset(buf,'\0',sizeof(buf));
    client = (int) thread_arg;
	//int box[1024];//存储从客户端接收到的数据
	//if(n = recv(client, box, sizeof(box), 0)>0){ //接收客户机数据
	  if(n=get_line(client, buf, sizeof (buf))>0){

		while(!isspace((int)buf[i])){
			i++;
			//printf("execute\n");
		}
		i++;
		while(!isspace((int)buf[i])){
			url[j]=buf[i];
			i++;
			j++;
		}
		
	if (url[strlen(url)- 1] == '/'){
		rootsend(client);//如果是请求根目录，就显示docroot目录下的文件列表
	}else{
		
		  strcat(paths,url);
		  printf("%s\n",paths);
		  serve_file(client, paths); 
	  }
	}
    close(client);
    return (NULL);
}
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
void rootsend(int client){
	
	char buf[1024*2048];
	List *head;
	char *docpath;
	docpath=DOCPATH;
	//getpath=getrunpath();
	head=getdirlist(docpath);//得到保存文件名的指针
    memset(buf,'\0',sizeof(buf));//全部置空。
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
	strcat(buf,docpath);
	strcat(buf,"</title> <meta http-equiv='Content-Type' content='text/html; charset=utf-8' /></head><body>");
	strcat(buf,"<h1>Index of ");
	strcat(buf,"/docroot");
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
	strcat(buf,"</table>tristan's server (Ubuntu) Server 2016</address></body></html>");
	write(client,buf,sizeof(buf));
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
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, or a CRLF combination.  Terminates the string read
 * with a null character.  If no newline indicator is found before the
 * end of the buffer, the string is terminated with a null.  If any of
 * the above three line terminators is read, the last character of the
 * string will be a linefeed and the string will be terminated with a
 * null character.
 * Parameters: the socket descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null) */
int get_line(int sock, char *buf, int size) {
    int i = 0;
    char c = '\0';
    int n;

    while ((i < size - 1) && (c != '\n')) {
        n = recv(sock, &c, 1, 0);
        /* DEBUG printf("%02X\n", c); */
        if (n > 0) {
            if (c == '\r') {
                n = recv(sock, &c, 1, MSG_PEEK);
                /* DEBUG printf("%02X\n", c); */
                if ((n > 0) && (c == '\n'))
                    recv(sock, &c, 1, 0);
                else
                    c = '\n';
            }
            buf[i] = c;
            i++;
        } else
            c = '\n';
    }
    buf[i] = '\0';

    return (i);
}
void serve_file(int client, const char *filename) {
    //返回文件数据
    FILE *resource = NULL;
    int numchars = 1;
    char buf[1024];

    buf[0] = 'A';
    buf[1] = '\0';
    while ((numchars > 0) && strcmp("\n", buf)) /* read & discard headers */
        numchars = get_line(client, buf, sizeof (buf));

    resource = fopen(filename, "r");
    if (resource == NULL){
		printf("file not find\n");
		headers(client, filename);
		return;
		//t_found(client);
	}else{
        //先返回头部信息
        headers(client, filename);
        cat(client, resource);
    }
    fclose(resource);
}
void headers(int client, const char *filename) {
    //先返回文件头部信息
    char buf[1024];
    (void) filename; /* could use filename to determine file type */

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: multipart/form-data \r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
}
void cat(int client, FILE *resource) {
    //返回文件数据
    char buf[1024];

    fgets(buf, sizeof (buf), resource);
    while (!feof(resource)) {
        //循环发送文件
        send(client, buf, strlen(buf), 0);
        fgets(buf, sizeof (buf), resource);
    }
}