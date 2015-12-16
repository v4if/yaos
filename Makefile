#!Makefile 

#基于依赖递归更新的编译规则

#patsubt 模式字符串替换，在C_SRC字列中的字（一列文件名），如果结尾是'.c'，就用'.o'取代'.c'
C_SRC=$(shell find . -name "*.c")
C_OBJ=$(patsubst %.c,%.o,$(C_SRC))
S_SRC=$(shell find . -name "*.s")
S_OBJ=$(patsubst %.s,%.o,$(S_SRC))

CC=gcc
LD=ld
ASM=nasm

C_FLAGS=-c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-builtin -fno-stack-protector -I include 
LD_FLAGS=-T scripts/kernel.ld -m elf_i386 -nostdlib
ASM_FLAGS = -f elf -g -F stabs

all:$(S_OBJ) $(C_OBJ) link update_image

#模式规则
#$<为所有依赖文件，$@为所有目标文件，相当于foreach处理操作
%.o:%.c
	@echo 编译代码文件...
	$(CC) $(C_FLAGS) $< -o $@
%.o:%.s
	@echo 编译汇编文件...
	$(ASM) $(ASM_FLAGS) $<
link:
	@echo 链接内核文件...
	$(LD) $(LD_FLAGS) $(S_OBJ) $(C_OBJ) -o atkos

#隐晦规则，伪目标文件
.PHONY:update_image
update_image:
	sudo mount floppy.img /mnt/kernel
	sudo cp atkos /mnt/kernel/
	sleep 1
	sudo umount /mnt/kernel

.PHONY:mount_image
mount_image:
	sudo mount floppy.img /mnt/kernel

.PHONY:umount_image
umount_image:
	sudo umount /mnt/kernel

.PHONY:qemu
qemu:
	qemu-system-i386 -fda floppy.img -boot a -m 64 -localtime

.PHONY:bochs
bochs:
	bochs -f scripts/bochsrc.txt

.PHONY:debug
debug:
	qemu -S -s -fda floppy.img -boot a &
	sleep 1
	cgdb -x scripts/gdbinit

#-rm 某些文件出现问题，但不要管，继续做后面的事
.PHONY:clean
clean:
	-rm -f $(S_OBJ) $(C_OBJ) atkos
