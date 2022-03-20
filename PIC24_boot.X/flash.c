
#include "xc.h"
#include "flash.h"

extern unsigned char pgm_buff[PGM_PAGE_BYTES];

// uncomment to write empty user application function
// void __attribute__((address(USER_CODE_START))) usr () { while (1); }

void write_buf_pgm(register unsigned int offset, unsigned int count) {
    register unsigned int z = count;

    // TBLPAG register is already set in calling function
    NVMCON = 0x405A; // Erase 4 rows of flash memory
    __builtin_tblwtl(offset, offset);
    asm volatile ("disi #5");
    __builtin_write_NVM();
    // flash program or erase operation stalls processor and 
    // NVMCONbits.WR bit is cleared when operation is finished

    NVMCON = 0x4004; // Write row of flash memory
    while (z) {
        // z has count of bytes in buffer with three bytes per instruction
        register unsigned int wr = pgm_buff[--z];
        wr |= (pgm_buff[--z] << 8);
        __builtin_tblwtl(offset, wr);
        wr = pgm_buff[--z];
        __builtin_tblwth(offset, wr);
        offset += 2;
        if ((offset & PGM_ROW_MASK) == 0) {
            // write flash memory row
            asm volatile ("disi #5");
            __builtin_write_NVM();
        }
    }
}

void write_buf_eem(register unsigned int offset, unsigned int count) {
    register unsigned int z = count;
    unsigned int * ptr = (unsigned int *) pgm_buff;

    // TBLPAG register is already set in calling function
    NVMCON = 0x4004; // Write EEPROM word with erase-before-write
    while (z) {
        // z has count of words to write with two bytes per EEPROM word in buffer
        register unsigned int wr = ptr[--z];
        if (wr != 0xFFFF) {
            __builtin_tblwtl(offset, wr);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            // wait while finished - program execution is not stopped
            // during a data EEPROM program or erase operation.
            while (NVMCONbits.WR);
        }
        offset += 2;
    }
}
