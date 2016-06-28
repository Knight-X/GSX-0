#include <stddef.h>
#include <stdint.h>
#include "reg.h"
#include "task.h"
#include "os.h"
#include "list.h"
#include "filesystem.h"
#include "fio.h"
#include "romfs.h"

#define CPU_CLOCK_HZ 72000000
#define TICK_RATE_HZ 10
#define USART_FLAG_TXE ((uint16_t) 0x0080)

struct list readyList[PRIORITY_LIMIT + 1];

extern uint32_t _sromfs;
void usart_init(void)
{
    *(RCC_APB2ENR) |= (uint32_t) (0x00000001 | 0x00000004);
    *(RCC_APB1ENR) |= (uint32_t) (0x00020000);

    *(GPIOA_CRL) = 0x00004B00;
    *(GPIOA_CRH) = 0x44444444;
    *(GPIOA_ODR) = 0x00000000;
    *(GPIOA_BSRR) = 0x00000000;
    *(GPIOA_BRR) = 0x00000000;

    *(USART2_CR1) = 0x0000000C;
    *(USART2_CR2) = 0x00000000;
    *(USART2_CR3) = 0x00000000;
    *(USART2_CR1) |= 0x2000;

}

void print_str(const char *str)
{
    while (*str) {
        while (!(*(USART2_SR) & USART_FLAG_TXE));
        *(USART2_DR) = (*str & 0xFF);
        str++;
    }
}

static void delay(volatile int count)
{
    count *= 50000;
    while (count--);
}

void busy_loop(void *str)
{
    while (1) {
        print_str(str);
        print_str(": Running....\n");
        delay(1000);
    }
}


void test1(void *userdata)
{
    busy_loop(userdata);
}

void test2(void *userdata)
{
    busy_loop(userdata);
}

void test3(void *userdata)
{
    busy_loop(userdata);
}

int main(void)
{
    const char *str1 = "Task1", *str2 = "Task2", *str3 = "Task3";


    usart_init();

    fio_init();
    fs_init();
    register_romfs("romfs", &_sromfs);
    for (int i = 0; i < PRIORITY_LIMIT; i++) {
        list_init(&readyList[i]);
    }
    if (task_create(test1, (void *)str1, 0) == -1)
        print_str("Failed for test1\n");

    if (task_create(test2, (void *)str2, 1) == -1)
        print_str("Failed for test2\n");
    
    if (task_create(test3, (void *)str3, 1) == -1)
        print_str("Failed for test2\n");

    *SYSTICK_LOAD = (CPU_CLOCK_HZ / TICK_RATE_HZ) - 1UL;
    *SYSTICK_VAL = 0;
    *SYSTICK_CTRL = 0x07;

    task_start();
    return 0;
}
