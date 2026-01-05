#include "../include/screen.h"
#include "../include/util.h"
#include "mem.h"
#include "../drivers/ata.h"
#include "../fs/fat12.h"
#include "usermode.h"
#include "process.h"

void shell_init() {
    kprint("MehdiOS Shell > ");
}

void user_input(char *input) {
    if (strcmp(input, "help") == 0) {
        kprint("Available commands:\n");
        kprint("  help     - Show this message\n");
        kprint("  clear    - Clear the screen\n");
        kprint("  echo     - Print text back\n");
        kprint("  ls       - List files\n");
        kprint("  mem      - Test memory allocation\n");
        kprint("  readsec  - Read boot sector\n");
        kprint("  usermode - Jump to Ring 3 (User Mode)\n");
        kprint("  ps       - List running processes\n");
    } else if (strcmp(input, "clear") == 0) {
        clear_screen();
        kprint_at("MehdiOS Shell > ", 0, 0);
        return;
    } else if (strcmp(input, "echo") == 0) {
        kprint("\n");
    } else if (input[0] == 'e' && input[1] == 'c' && input[2] == 'h' && input[3] == 'o' && input[4] == ' ') {
        kprint(input + 5);
        kprint("\n");
    } else if (strcmp(input, "mem") == 0) {
        kprint("Testing memory allocation...\n");
        uint32_t phys_addr;
        uint32_t page = kmalloc(1000, 1, &phys_addr);
        kprint("Allocated 1000 bytes at: ");
        char str[16];
        hex_to_ascii(page, str);
        kprint(str);
        kprint("\n");
    } else if (strcmp(input, "readsec") == 0) {
        kprint("Reading Sector 0 (Bootloader)...\n");
        uint8_t buffer[512];
        ata_read_sectors(0, 1, buffer);
        kprint("First 32 bytes (Hex):\n");
        char hex[10];
        for(int i=0; i<32; i++) {
            hex_to_ascii(buffer[i], hex);
            kprint(hex);
            kprint(" ");
        }
        kprint("\n");
        if(buffer[510] == 0x55 && buffer[511] == 0xAA) {
            kprint("Valid Boot Sector Signature Found!\n");
        }
    } else if (strcmp(input, "ls") == 0) {
        fat_init();
        fat_list_root();
    } else if (strcmp(input, "usermode") == 0) {
        kprint("Switching to User Mode (Ring 3)...\n");
        kprint("User program will use INT 0x80 to call kernel.\n");
        switch_to_user_mode();
        kprint("ERROR: Returned from user mode!\n");
    } else if (strcmp(input, "ps") == 0) {
        process_list();
    } else {
        kprint("Unknown command: ");
        kprint(input);
        kprint("\nType 'help' for commands.\n");
    }

    kprint("> ");
}
