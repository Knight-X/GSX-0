#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

void push(stack *st, struct node *element) {
	st->head->next = element;
	element->prev = st->head;
	element->next = NULL;
	st->head = element;
}

struct node* pop(stack *st) {
	struct node *tmp = st->head;
	st->head = tmp->prev;
	tmp->prev->next = NULL;
	tmp->prev = NULL;
	return tmp;
}
void init(stack *st, struct node *element) {
	st->head = element;
	st->head->next = element;
	st->head->prev = element;
}

