CROSS_COMPILE = arm-none-eabi-

CC := $(CROSS_COMPILE)gcc

CFLAGS = -fno-common -ffreestanding -O0 -std=gnu99 \
	 -gdwarf-2 -g3 -Wall -Werror \
	 -mcpu=cortex-m3 -mthumb \
	 -Wl,-Tos.ld -nostartfiles

TARGET = os.bin
all: $(TARGET)

$(TARGET): test-romfs.o os.c init.c malloc.c task.c list.c hash-djb2.c romfs.c fio.c filesystem.c syscall.S
	$(CC) $(CFLAGS) $^ -o os.elf
	$(CROSS_COMPILE)objcopy -Obinary os.elf os.bin
	$(CROSS_COMPILE)objdump -S os.elf > os.list

test: test.c task.c
	$(CC) $(CFLAGS) $^ -o task.elf
	$(CROSS_COMPILE)objcopy -Obinary task.elf task.bin
CPU=arm
TARGET_FORMAT = elf32-littlearm
TARGET_OBJCOPY_BIN = $(CROSS_COMPILE)objcopy -I binary -O $(TARGET_FORMAT) --binary-architecture $(CPU)

mkromfs:
	gcc -o mkromfs mkromfs.c

test-romfs.o: mkromfs
	./mkromfs -d test-romfs test-romfs.bin
	$(TARGET_OBJCOPY_BIN) --prefix-sections '.romfs' test-romfs.bin test-romfs.o

qemu: $(TARGET)
	@echo "Press Ctrl-A and then X to exit QEMU"
	@echo
	qemu-system-arm -M stm32-p103 -nographic -kernel os.bin
qemudbg: $(TARGET) 
	qemu-system-arm -M stm32-p103 \
		-gdb tcp::3333 -S \
		-kernel os.bin
clean:
	rm -f *.o *.elf *.bin *.list

