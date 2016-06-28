CROSS_COMPILE = arm-none-eabi-

CC := $(CROSS_COMPILE)gcc

CFLAGS = -fno-common -ffreestanding -O0 -std=gnu99 \
	 -gdwarf-2 -g3 -Wall -Werror \
	 -mcpu=cortex-m3 -mthumb \
	 -Wl,-Tos.ld -nostartfiles

TARGET = os.bin
all: $(TARGET)

$(TARGET): os.c init.c malloc.c task.c list.c hash-djb2.c filesystem.c fio.c romfs.c
	$(CC) $(CFLAGS) $^ -o os.elf
	$(CROSS_COMPILE)objcopy -Obinary os.elf os.bin
	$(CROSS_COMPILE)objdump -S os.elf > os.list

test: test.c task.c
	$(CC) $(CFLAGS) $^ -o task.elf
	$(CROSS_COMPILE)objcopy -Obinary task.elf task.bin

qemu: $(TARGET)
	@qemu-system-arm -M ? | grep stm32-p103 > /dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	@echo
	qemu-system-arm -M stm32-p103 -nographic -kernel os.bin

clean:
	rm -f *.o *.elf *.bin *.list

