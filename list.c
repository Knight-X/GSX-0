#include "list.h"

void list_init(struct list *list)
{
    if (list) {
        list->prev = list;
        list->next = list;
    }
}

int list_empty(struct list *list)
{
    return list->next == list;
}
void list_push(struct list *list, struct list *node)
{
    if (list && node) {

        list_remove(node);

        list->prev->next = node;
        node->prev = list->prev;
        list->prev = node;
        node->next = list;
    }
}

void list_insert(struct list *list, struct list *node) 
{
    if (list && node) {
        list_remove(node);

        list->next->prev = node;
        node->next = list->next;
        list->next = node;
        node->prev = list;
    }
}

struct list *list_shift(struct list *list)
{
    struct list *node = list->next;
    
    if (node == list)
        return 0;

    list->next = node->next;
    list->next->prev = list;

    return node;
}


void list_remove(struct list *list)
{
    if (list) {
        list->prev->next = list->next;
        list->next->prev = list->prev;
    }
}
