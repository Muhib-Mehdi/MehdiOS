/* Linker Script for our 32-bit Kernel */
ENTRY(_start)

SECTIONS
{
    /* Begin putting sections at 1 MiB, a conventional place for kernels to be loaded by bootloaders 
       BUT we are loading at 0x1000 in our custom bootloader, so we must link to 0x1000 (4KB). 
       Verify boot_sect.asm KERNEL_OFFSET. It is 0x1000. 
    */
    . = 0x1000;

    /* Text section (Executable code) */
    .text : ALIGN(4)
    {
        *(.text)
    }

    /* Read-only data (Constants) */
    .rodata : ALIGN(4)
    {
        *(.rodata)
    }

    /* Data section (Initialized variables) */
    .data : ALIGN(4)
    {
        *(.data)
    }

    /* BSS section (Uninitialized variables) */
    .bss : ALIGN(4)
    {
        *(COMMON)
        *(.bss)
    }
}
