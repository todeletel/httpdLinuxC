//Linux C系统编程学习程序  start at:2016/12/22
#include <unistd.h>  // 实现POSIX标准的OS 调用其API的头文件
#include <sys/types.h>
#include <dirent.h> //opendir 的库函数
#include "list.h"//自己定义数据结构
List * getdirlist(char *path);//得到当前目录的文件列表
char * getrunpath();//得到当前目录的路径；


List * getdirlist(char *path){//12/22
	struct dirent *entry;//目录项实体
	DIR *dir=NULL;
	List *L=initList();//初始化链表用于保存文件名列表。
	List *p;//保存分配结点的指针。
	dir=opendir(path);
	//errno=0;
	//将得到文件保存到链表中
	
	
	while((entry=readdir(dir))!=NULL){
		//if(strcmp(entry->d_name,".")&&strcmp(entry->d_name,".."))
		p=malloc(sizeof(List));
		p->filename=entry->d_name;  
		p->next=L->next;  //头插法将结点插入链表
		L->next=p;
		//printf("%s\n",entry->d_name);
	}
	
	closedir(dir);
	return L;
 }
char* getrunpath(){//得到当前程序运行的绝对路径   2016/12/22  使用getcwd库函数
	static char path[255];
	
	getcwd(path,255);//函数声明 char * getcwd(char * buf,size_t size);
	if(!path){
		perror("getcwd");
		exit(1);
	}
	printf("cwd=%s\n",path);
	return path;
}
/*int main(){

	//if(dir!=NULL) printf("打开目录流成功\n");
	
	List *head;
	char *getpath;
	getpath=getrunpath();
	head=getdirlist(getpath);//得到保存文件名的指针
	
	while(head->next!=NULL){
		head=head->next;
		printf("%s\n",head->filename);
	}
	
	//char *getpath;
	//getpath=getrunpath();
	//getdirlist(getpath);
	
	return 0;
}*/