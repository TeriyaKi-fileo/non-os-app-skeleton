# ツール
CC      := gcc
LD      := ld
AS      := nasm

# フォルダ
INC_DIR := include
SRC_DIR := src
OBJ_DIR := obj
SRCS    := $(wildcard $(SRC_DIR)/*.c)
OBJS    := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
# kernel.o を独立させて管理する
KERNEL_OBJ := $(OBJ_DIR)/kernel.o
OTHER_OBJS := $(filter-out $(KERNEL_OBJ), $(OBJS))

# ファイル名
BOOT    := boot.bin
KERNEL  := kernel.bin
IMG     := floppy.img
LINKER  := linker.ld

# オプション
CFLAGS  = -m32 -Werror -ffreestanding -fno-stack-protector -fno-pic -O1 -I$(INC_DIR)
LDFLAGS = -m elf_i386 -T $(LINKER)

all: $(IMG)

$(OBJ_DIR)/$(BOOT): boot.asm
	mkdir -p $(OBJ_DIR)
	$(AS) -f bin $< -o $@

$(OBJ_DIR)/$(KERNEL): $(KERNEL_OBJ) $(OTHER_OBJS)
	mkdir -p $(OBJ_DIR)
	$(LD) $(LDFLAGS) $(KERNEL_OBJ) $(OTHER_OBJS) -o $@

$(IMG): $(OBJ_DIR)/$(BOOT) $(OBJ_DIR)/$(KERNEL)
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=$(OBJ_DIR)/$(BOOT) of=$(IMG) bs=512 count=1 conv=notrunc
	dd if=$(OBJ_DIR)/$(KERNEL) of=$(IMG) bs=512 seek=1 conv=notrunc

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(IMG)

qemu: $(IMG)
	qemu-system-i386 -drive format=raw,file=$(IMG),if=floppy,index=0 \
	-machine pcspk-audiodev=snd0 -vga std \
	-audiodev pa,id=snd0

bochs: $(IMG)
	bochs -q -f non-os.bochsrc
