#include <stdint.h>
#include "task.h"
#include "reg.h"
#include "malloc.h"
#include "os.h"
#include "list.h"
#include "fio.h"

#define TASK_PSP 0XFFFFFFFD

int pr = 0;

int timeup = 0;
typedef struct{
    void *stack;
    void *orig_stack;
    uint8_t in_use;
    uint8_t tid;
    int priority;
    struct list listNode;
} tcb_t;

static tcb_t tasks[MAX_TASKS];
static int first = 1;
static int lastTask;
extern struct list readyList[PRIORITY_LIMIT + 1];

int task_create(void (*run)(void *), void *userdata, int priority)
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
    tasks[taskId].priority = priority;
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
    list_init(&tasks[taskId].listNode);
    list_push(&readyList[tasks[taskId].priority], &tasks[taskId].listNode);


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
    list_remove(&tasks[task_id].listNode);
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
}*/

void __attribute__((naked)) svc_handler()
{
   // unsigned int x = ((unsigned int *)gg)[3];
    asm volatile("mrs r0, psp\n"
		"stmdb r0!, {r4-r11, lr}\n");
    tcb_t *task = &tasks[lastTask];
    uint32_t *gg = (uint32_t *)task->stack;

    asm volatile("mov %0, r0\n" : "=r" (tasks[lastTask].stack));
    if ((int)gg[3] == 0x4) {
	int fd = (int)gg[9];
	char * buf = (char *)gg[10];
	fio_read(fd, buf, sizeof(buf));
	print_str(buf);
	//tasks[lastTask].stack[9] = count;
    }
    asm volatile("mov r0, %0\n" : : "r" (tasks[lastTask].stack));
    asm volatile("ldmia r0!, {r4-r11, lr}\n");
    asm volatile("msr psp, r0\n");
    asm volatile("bx lr\n");
             
}

void __attribute__((naked)) pendsv_handler()
{
    tcb_t *task;

    asm volatile("mrs r0, psp\n"
                 "stmdb r0!, {r4-r11, lr}\n");

    asm volatile("mov %0, r0\n" : "=r" (tasks[lastTask].stack));
    task = &tasks[lastTask];
    if (timeup && readyList[task->priority].next == &task->listNode) {
        list_push(&readyList[task->priority], &task->listNode);
        timeup = 0;
    }

//    for (pr = 0; list_empty(&readyList[pr]); pr++);

    if (pr == 1) 
        pr = 0;
    else 
        pr = 1;

    struct list *list = readyList[pr].next;
    task = LIST_ENTRY(list, tcb_t, listNode);
    lastTask = task->tid;

    asm volatile("mov r0, %0\n" : : "r" (tasks[lastTask].stack));

    asm volatile("ldmia r0!, {r4-r11, lr}\n"
                 "msr psp, r0\n"
                 "bx lr\n");
}

void systick_handler()
{
    timeup = 1;
    *SCB_ICSR |= SCB_ICSR_PENDSVSET;
}
