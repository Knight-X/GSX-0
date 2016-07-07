#ifndef __Task_H_
#define __Task_H_
#include "list.h"
#include <stdint.h>

void task_start();
int task_create(void (*run)(void *), void *stack, int priority);
void task_kill(int task_id);
int context_switch(void);
typedef struct{
    void *stack;
    void *orig_stack;
    uint8_t in_use;
    uint8_t tid;
    int priority;
    struct list listNode;
} tcb_t;

#endif
