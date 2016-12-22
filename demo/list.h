//自己实现的数据结构
typedef struct List{    /* 定义单链表结点类型 */
    char *filename;
    struct list *next;
}List;
/* 1.初始化线性表，即置单链表的表头指针为空 */
List * initList()
{	
	List *L=malloc(sizeof(List));
    L->filename=NULL;
	L->next=NULL;
    //printf("initList函数执行，初始化成功\n");
	return L;
}
