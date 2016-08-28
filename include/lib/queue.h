#ifndef __QUEUE__H_
#define __QUEUE_H_

struct node{
	struct node *next;
	int len;
};

typedef struct node qnode;

struct queue {
	qnode *head;
	qnode *tail;
};	
struct Queue {
	struct queue *q;
	int (*push)(struct Queue *list, int len);
	int (*pop)(struct Queue *list);
};

int init(struct Queue **self);
int push(struct Queue *list, int len);
int pop(struct Queue *list);
#endif
