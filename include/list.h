#ifndef __LIST_H_
#define __LIST_H_

#include "utils.h"

#define LIST_ENTRY(ptr, type, member) \
    (container_of((ptr), type, member))

struct list {
    struct list *prev;
    struct list *next;
};


void list_init(struct list *list);
int list_empty(struct list *list);
void list_push(struct list *list, struct list *nlist);
void list_remove(struct list *list);
struct list* list_shift(struct list *list);
void list_insert(struct list *list, struct list *node);



#endif
