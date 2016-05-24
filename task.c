#include <stdint.h>
#include "task.h"
#include "reg.h"
#include "malloc.h"
#include "os.h"
#include "list.h"


#define TASK_PSP 0XFFFFFFFD


typedef struct{
    void *stack;
    void *orig_stack;
    uint8_t in_use;
    uint8_t tid;
    struct list info;
} tcb_t;

static tcb_t tasks[MAX_TASKS];
static int first = 1;
static int lastTask;
//extern struct list ready_list[PRIORITY_LIMIT + 1];

int task_create(void (*run)(void *), void *userdata)
{
    uint32_t *stack;
    int taskId = 0;

    for (taskId = 0; taskId < MAX_TASKS; taskId++) {
        if (tasks[taskId].in_use == 0)
            break;
    }
    stack = malloc(STACK_SIZE * sizeof(uint32_t));
    tasks[taskId].orig_stack = stack;
    tasks[taskId].tid = taskId;
    stack += STACK_SIZE - 32;

    if (first == 1) {
        stack[8] = (unsigned int ) run;
        stack[9] = (unsigned int) userdata;
        first = 0;
    } else {
        stack[8] = (unsigned int) TASK_PSP;
        stack[9] = (unsigned int) userdata;
        stack[14] = (unsigned) &task_kill;
        stack[15] = (unsigned int) run;
        stack[16] = (unsigned int) 0x21000000;
    }

    tasks[taskId].stack = stack;
    tasks[taskId].in_use = 1;

    return taskId;
}


void task_start()
{
    lastTask = 0;
    asm volatile("mrs ip, psr\n"
                 "push {r4-r11, ip, lr}\n");

    asm volatile("mov r0, %0\n" : : "r" (tasks[lastTask].stack));

    asm volatile("msr psp, r0\n"
                 "mov r0, #3\n"
                 "msr control, r0\n"
                 "isb\n"
                 "pop {r4-r11, lr}\n"
                 "pop {r0}\n"
                 "bx lr");
}

void task_kill(int task_id)
{
    asm volatile("cpsid i\n");
    tasks[task_id].in_use = 0;

    free(tasks[task_id].orig_stack);
    asm volatile("cpsid i\n");
}
/*
int context_switch()
{
    int i;
    for (i = 0; list_empty(&ready_list[i]); i++);
    tcb_t *task;
    struct list *list;
    list = ready_list[i].next;
    task = LIST_ENTRY(list, tcb_t, info);
    return task->tid;
}
*/
void __attribute__((naked)) pendsv_handler()
{

    asm volatile("mrs r0, psp\n"
                 "stmdb r0!, {r4-r11, lr}\n");

    asm volatile("mov %0, r0\n" : "=r" (tasks[lastTask].stack));

    if (lastTask == 0) 
        lastTask = 1;
    else 
        lastTask = 0;

    asm volatile("mov r0, %0\n" : : "r" (tasks[lastTask].stack));

    asm volatile("ldmia r0!, {r4-r11, lr}\n"
                 "msr psp, r0\n"
                 "bx lr\n");
}

void systick_handler()
{
    *SCB_ICSR |= SCB_ICSR_PENDSVSET;
}
