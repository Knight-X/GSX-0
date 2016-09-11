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
	int (*enqueue)(struct Queue *list, int len);
	int (*dequeue)(struct Queue *list);
};

#endif
