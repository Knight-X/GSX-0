CROSS_COMPILE = arm-none-eabi-

CC := $(CROSS_COMPILE)gcc

out = build

includes = \
	include \

CFLAGS_INCLUDE = $(foreach i, $(includes), -I$(i))

CFLAGS = -fno-common -ffreestanding -O0 -std=gnu99 \
	 -gdwarf-2 -g3 -Wall -Werror \
	 -mcpu=cortex-m3 -mthumb \
	 -Wl,-Tos.ld -nostartfiles $(CFLAGS_INCLUDE) \

TARGET = os

include kernel/build.mk

objs_from_dir = $(foreach obj, $($(2)-y), \
	$(out)/$(1)/$(obj))

all-y += $(call objs_from_dir,kernel,kernel)

.PHONY: all
all: $(out)/$(TARGET).bin

$(out)/os.bin: $(out)/os.elf
	$(CROSS_COMPILE)objcopy -Obinary $< $@

$(out)/os.list:$(out)/os.elf
	$(CROSS_COMPILE)objdump -S $< > $@

$(out)/os.elf: $(all-y)
	$(CC) $(CFLAGS) $(all-y) -o $@

$(out)/%.o:%.c
	$(CC) $(CFLAGS) -MMD -MF $@.d -c $< -o $@

$(out)/%.o:%.S
	$(CC) $(CFLAGS) -MMD -MF $@.d -c $< -o $@

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

qemu: $(out)/$(TARGET).bin
	@echo "Press Ctrl-A and then X to exit QEMU"
	@echo
	qemu-system-arm -M stm32-p103 -nographic -kernel build/os.bin
qemudbg: $(TARGET) 
	qemu-system-arm -M stm32-p103 \
		-gdb tcp::3333 -S \
		-kernel build/os.bin
exec:
	@cd /home/kazami; pwd
clean:
	rm -f *.o *.elf *.bin *.list

