#ifndef __Task_H_
#define __Task_H_

void task_start();
int task_create(void (*run)(void *), void *stack, int priority);
void task_kill(int task_id);
int context_switch(void);

#endif
