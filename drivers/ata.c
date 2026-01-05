#include "ata.h"
#include "../include/ports.h"
#include "../include/screen.h"

/*
 * Waits until the BSY (Busy) bit is cleared and the DRQ (Data Request) bit is set.
 */
void ata_wait_bsy() {
    while(port_byte_in(ATA_STATUS) & ATA_SR_BSY);
}

void ata_wait_drq() {
    while(!(port_byte_in(ATA_STATUS) & ATA_SR_DRQ));
}

void ata_read_sectors(uint32_t lba, uint8_t count, uint8_t* buffer) {
    // 0xE0 = 11100000 = Master Drive, LBA Mode
    // We mask the top 4 bits of the LBA (bits 24-27) into the drive head port
    port_byte_out(ATA_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
    
    // Send NULL byte to Error port (standard procedure)
    port_byte_out(ATA_ERROR, 0x00);
    
    // Sector Count
    port_byte_out(ATA_SECTOR_CNT, count);
    
    // LBA Low (bits 0-7)
    port_byte_out(ATA_LBA_LO, (uint8_t)(lba & 0xFF));
    // LBA Mid (bits 8-15)
    port_byte_out(ATA_LBA_MID, (uint8_t)((lba >> 8) & 0xFF));
    // LBA High (bits 16-23)
    port_byte_out(ATA_LBA_HI, (uint8_t)((lba >> 16) & 0xFF));
    
    // Send Read Command
    port_byte_out(ATA_COMMAND, ATA_CMD_READ_PIO);
    
    // Read the data
    uint16_t *target = (uint16_t*) buffer;
    
    for (int j = 0; j < count; j++) {
        ata_wait_bsy();
        ata_wait_drq();
        
        for(int i = 0; i < 256; i++) {
            // Read Word (2 bytes) at a time
            target[i] = port_word_in(ATA_DATA);
        }
        target += 256;
    }
}
