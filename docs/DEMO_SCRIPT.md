# Portfolio Demo Script (2-3 Minutes)

Use this script to record your video for the MIT application.

## Scene 1: Introduction (0:00 - 0:30)
**Visual**: Show the code editor with the project structure open.
**Voiceover**:
"Hi, I'm [Your Name]. This is MehdiOS, a 32-bit operating system I built from scratch in C and Assembly to understand the fundamentals of computer architecture. Unlike standard tutorials, I implemented my own bootloader, interrupt handling system, and file system driver without relying on GRUB or external libraries."

## Scene 2: The Boot Process (0:30 - 1:00)
**Action**: Run `make run` in the terminal.
**Visual**: Show QEMU window popping up, "Starting MehdiOS Kernel..." appearing.
**Voiceover**:
"Here you see the boot process. My custom Assembly bootloader loads the kernel from the disk, switches the CPU from 16-bit Real Mode to 32-bit Protected Mode, and initializes the GDT. You can see the kernel banner and the prompt, proving we have successfully jumped to C code."

## Scene 3: Interrupts & Input (1:00 - 1:45)
**Action**: Type `help` in the shell. Then type `echo Hello MIT`.
**Visual**: Show the shell responding to commands.
**Voiceover**:
"I implemented a fully interrupt-driven keyboard driver. When I type, the hardware fires IRQ1, which my Interrupt Descriptor Table (IDT) routes to a C handler. This allows for an interactive shell. I also wrote a memory manager—watching this..."
**Action**: Type `mem`.
**Visual**: Show the memory allocation address.
**Voiceover**:
"This command demonstrates dynamic memory allocation, returning a physical address from my heap manager."

## Scene 4: Drivers & Code Quality (1:45 - 2:15)
**Action**: Type `readsec`.
**Visual**: Show the hex dump of the boot sector.
**Voiceover**:
"To interact with storage, I wrote a Protected Mode ATA driver that talks directly to the hard drive controller ports. Here it is reading the raw MBR sector from the disk. I structured the codebase professionally with modular drivers, a strict style guide, and comprehensive documentation to ensure maintainability."

## Scene 5: Conclusion (2:15 - 2:30)
**Visual**: Briefly scroll through `kernel.c` or `boot_sect.asm`.
**Voiceover**:
"Building this OS taught me how software truly interacts with hardware, from the boot sector to the shell. Thank you for watching."
