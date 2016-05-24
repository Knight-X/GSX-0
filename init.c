#include "reg.h"
#include "init.h"
#include <stdint.h>


extern void main(void);

extern uint32_t _sinidata;

extern uint32_t _sdata;

extern uint32_t _edata;

extern uint32_t _sbss;

extern uint32_t _ebss;

extern uint32_t _estack;

__attribute((section(".isr_vector")))
uint32_t *isr_vectors[] = {
    [0x00] = (uint32_t *) &_estack,
    [0x01] = (uint32_t *) reset_handler,
    [0x02] = (uint32_t *) nmi_handler,
    [0x03] = (uint32_t *) hardfault_handler,
    [0x05] = (uint32_t *) memmanage_handler,
    [0x06] = (uint32_t *) usagefault_handler,
    [0x0B] = (uint32_t *) svc_handler,
    [0x0E] = (uint32_t *) pendsv_handler,
    [0x0F] = (uint32_t *) systick_handler
};


void reset_handler(void)
{
    uint32_t *initdata_start = &_sinidata;
    uint32_t *data_start = &_sdata;
    uint32_t *data_end = &_edata;
    while (data_start < data_end) *data_start++ = *initdata_start++;

    uint32_t *bss_start = &_sbss;
    uint32_t *bss_end = &_ebss;

    while (bss_start < bss_end) *bss_start++ = 0;

    main();

}


void rcc_clock_init(void)
{

    *RCC_CR |= (uint32_t) 0x00000001;

    *RCC_CFGR &= (uint32_t) 0x0F8FF0000;

    *RCC_CR &= (uint32_t) 0xFEF6FFFF;

    *RCC_CR &= (uint32_t) 0xFFFBFFFF;

    *RCC_CFGR &= (uint32_t) 0xFF80FFFF;

    *RCC_CIR = 0x009F0000;

    volatile uint32_t StartUpCounter = 0, HSEStatus = 0;

    *RCC_CR |= ((uint32_t)RCC_CR_HSEON);

    do {
        HSEStatus = *RCC_CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((*RCC_CR &RCC_CR_HSERDY) != 0)
        HSEStatus = (uint32_t) 0x01;
    else
        HSEStatus = (uint32_t) 0x00;

    if (HSEStatus == (uint32_t) 0x01) {

        *FLASH_ACR &= (uint32_t)((uint32_t) ~FLASH_ACR_LATENCY);

        *FLASH_ACR |= (uint32_t)FLASH_ACR_LATENCY;

        *RCC_CFGR |= (uint32_t) RCC_CFGR_HPRE_DIV1;
        *RCC_CFGR |= (uint32_t) RCC_CFGR_PPRE2_DIV1;

        *RCC_CFGR |= (uint32_t) RCC_CFGR_PPRE1_DIV1;

        *RCC_CFGR &= (uint32_t)((uint32_t) ~(RCC_CFGR_SW));

        *RCC_CFGR |= (uint32_t) RCC_CFGR_SW_HSE;

        while ((*RCC_CFGR & (uint32_t) RCC_CFGR_SW) != (uint32_t) 0x04);
    
    } else {
    /*ready for fit*/
    }
}



