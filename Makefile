# Makefile for MyOS
# 
# Main targets:
#   all:     Builds the OS image (os-image.bin)
#   run:     Builds and runs in QEMU
#   debug:   Builds with debug symbols and runs QEMU in debug mode
#   clean:   Removes build artifacts

# Tools
CC = gcc
LD = ld
ASM = nasm
QEMU = qemu-system-x86_64

# Flags
# -m32: Compile for 32-bit mode
# -ffreestanding: No standard library
# -g: Generate debug information
# -fno-pie: Disable Position Independent Executable (important for kernel)
CFLAGS = -m32 -ffreestanding -g -c -fno-pie -I include
LDFLAGS = -m elf_i386 -T kernel/linker.ld
ASMFLAGS = -f elf

# Sources
C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c fs/*.c)
HEADERS = $(wildcard include/*.h cpu/*.h drivers/*.h fs/*.h kernel/*.h)
# Object files derived from C sources
OBJ = $(C_SOURCES:.c=.o) cpu/interrupt.o cpu/gdt_flush.o cpu/usermode.o cpu/switch.o

# Default target
all: os-image.bin

# Run in QEMU
run: all
	$(QEMU) -name "MehdiOS" -fda os-image.bin

# Debug in QEMU (Pauses for GDB)
# Connect with: gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"
debug: os-image.bin kernel.elf
	$(QEMU) -s -S -fda os-image.bin &

# Create the final disk image (Bootloader + Kernel)
os-image.bin: boot/boot_sect.bin kernel.bin
	cat $^ > os-image.bin

# Build the kernel binary
# We link the kernel entry point (ASM) with the C object files
kernel.bin: kernel/kernel_entry.o $(OBJ)
	$(LD) -o $@ $(LDFLAGS) $^ --oformat binary

# Keep the ELF version for debugging symbols
kernel.elf: kernel/kernel_entry.o $(OBJ)
	$(LD) -o $@ $(LDFLAGS) $^

# Rule for C files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

# Rule for Screen Drivers
# (Already covered by generic rule)

# Rule for Interrupt (ASM wrapper)
cpu/interrupt.o: cpu/interrupt.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Rule for GDT flush (ASM)
cpu/gdt_flush.o: cpu/gdt_flush.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Rule for User mode switch (ASM)
cpu/usermode.o: cpu/usermode.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Rule for Context switch (ASM)
cpu/switch.o: cpu/switch.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Rule for Kernel Entry (ASM)
kernel/kernel_entry.o: kernel/kernel_entry.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Rule for Boot Sector (Raw Binary)
boot/boot_sect.bin: boot/boot_sect.asm
	$(ASM) -f bin $< -o $@

# Boot sector dependency check (so it rebuilds if included files change)
boot/boot_sect.bin: boot/print_real_mode.asm boot/disk_load.asm boot/gdt.asm boot/switch_pm.asm

# Tool utilities
clean:
	rm -fr *.bin *.o *.elf os-image.bin
	rm -fr kernel/*.o drivers/*.o boot/*.bin boot/*.o

# Hexdump of the final image (for verification)
hexdump: os-image.bin
	od -t x1 -A n os-image.bin | head -n 40
