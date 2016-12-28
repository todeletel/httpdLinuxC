#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>//unix standard lib
//#include <string.h>
void * new(void *arg){
	int i;
	int a,n;
	a=*((int *)arg);
	n=a;
	for(i=0;i<n;i++)
	printf("from%dprint%d\n",a,i);
	sleep(10);
	printf("from%dprint after sleep\n",a);
	return NULL;
}
int main(){
	pthread_t thread;
	int ret;
	int a=0;
	while(a!=99){
		sleep(1);
		scanf("%d",&a);
		ret=pthread_create(&thread,NULL,new,&a);
	if(ret!=0){
		errno=ret;
		perror("pthread_create");
		//return -1;
		}
	}
	
	
	return 0;
}