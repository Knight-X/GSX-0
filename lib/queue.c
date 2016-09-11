#include "queue.h"
#include "malloc.h"

static int enqueue(struct Queue *q, int len) {
	qnode *new;
	if (0 == (new = (qnode *)malloc(sizeof(qnode)))) return -1;
	new->len = len;
	
	if (q->q->head == 0 && q->q->tail == 0) {
		q->q->tail = q->q->head = new;	
	}
	 else {
		q->q->head->next = new;	
		q->q->head = new;
	}
	new->next = 0;
	return 1;
}

static int dequeue(struct Queue *q) {
	qnode *final = q->q->tail;
	q->q->tail = q->q->tail->next;
	int d = final->len;
	free(final);
	return d;
}
	

int init(struct Queue **self) {
	if (0 == (*self = (struct Queue *)malloc(sizeof(struct Queue)))) return -1;
	(*self)->q = malloc(sizeof(struct queue));
	(*self)->q->head = 0;
	(*self)->q->tail = 0; 
	(*self)->enqueue = enqueue;
	(*self)->dequeue = dequeue;
	return 1;
}

