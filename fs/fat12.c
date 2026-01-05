#include "fat12.h"
#include "../drivers/ata.h"
#include "../include/screen.h"
#include "../include/util.h"
#include "../include/kernel.h"

fat_boot_sector_t bpb;
uint32_t root_dir_start_sector;
uint32_t data_start_sector;
uint32_t root_dir_sectors;

void fat_init() {
    // 1. Read the Boot Sector (Sector 0)
    uint8_t buffer[512];
    ata_read_sectors(0, 1, buffer);
    
    // 2. Cast to BPB struct
    memory_copy((char*)buffer, (char*)&bpb, sizeof(fat_boot_sector_t));
    
    // 3. Validate - check for valid FAT parameters
    if (bpb.bytes_per_sector == 0 || bpb.bytes_per_sector != 512) {
       kprint("Note: No FAT12 filesystem detected.\n");
       kprint("The boot disk is a raw kernel image.\n");
       kprint("To use ls, create a FAT12 disk image.\n");
       return;
    }
    
    if (bpb.table_size_16 == 0 || bpb.root_entry_count == 0) {
       kprint("Note: Invalid FAT12 parameters.\n");
       return;
    }

    // 4. Calculate Offsets
    root_dir_start_sector = bpb.reserved_sector_count + (bpb.table_count * bpb.table_size_16);
    root_dir_sectors = (bpb.root_entry_count * 32) / bpb.bytes_per_sector;
    if ((bpb.root_entry_count * 32) % bpb.bytes_per_sector != 0) root_dir_sectors++;
    data_start_sector = root_dir_start_sector + root_dir_sectors;

    kprint("FAT12 Initialized.\n");
    kprint("  Root Dir Sector: "); 
    char s[10]; int_to_ascii(root_dir_start_sector, s); kprint(s); kprint("\n");
}

void fat_list_root() {
    // Read Root Directory Sectors
    // For simplicity, we read one sector at a time
    uint8_t buffer[512];
    fat_directory_entry_t* entry;
    
    kprint("Listing Root Directory:\n");

    for (int i = 0; i < root_dir_sectors; i++) {
        ata_read_sectors(root_dir_start_sector + i, 1, buffer);
        entry = (fat_directory_entry_t*) buffer;
        
        // iterate 16 entries per sector (512 / 32 = 16)
        for (int j = 0; j < 16; j++) {
            if (entry[j].name[0] == 0x00) return; // End of directory
            if (entry[j].name[0] == 0xE5) continue; // Deleted file
            
            if (entry[j].attributes == 0x0F) continue; // Long File Name (skip for now)
            
            // Print Name
            char name[9];
            memory_copy((char*)entry[j].name, name, 8);
            name[8] = 0;
            // Trim spaces
            for(int k=7; k>=0; k--) {
                if(name[k] == ' ') name[k] = 0;
                else break;
            }
            kprint(name);
            
            // Print Extension (if not directory)
            if ((entry[j].attributes & 0x10) == 0) {
               kprint(".");
               char ext[4];
               memory_copy((char*)entry[j].ext, ext, 3);
               ext[3] = 0;
               kprint(ext);
            } else {
               kprint("/");
            }
            kprint("  ");
            
            // Print Size
            char size[10];
            int_to_ascii(entry[j].size, size);
            kprint(size);
            kprint(" bytes\n");
        }
    }
}
