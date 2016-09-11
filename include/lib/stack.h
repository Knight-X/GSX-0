#ifndef __STACK_H_
#define __STACK_H_

struct node {
	int len;
	struct node *next;
	struct node *prev;
};

typedef struct {
	struct node *head;
	} stack;

void init(stack *st, struct node *);
void push(stack *st, struct node *element);
struct node *pop(stack *st);

#endif
