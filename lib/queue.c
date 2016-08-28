#include "queue.h"
#include "malloc.h"

int init(struct Queue **self) {
	if (0 == (*self = (struct Queue *)malloc(sizeof(struct Queue)))) return -1;
	(*self)->q = malloc(sizeof(struct queue));
	(*self)->q->head = 0;
	(*self)->q->tail = 0; 
	(*self)->push = push;
	(*self)->pop = pop;
	return 1;
}

int push(struct Queue *q, int len) {
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
int pop(struct Queue *q) {
	qnode *final = q->q->tail;
	q->q->tail = q->q->tail->next;
	int d = final->len;
	free(final);
	return d;
}
	
