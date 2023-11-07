
/* 
 * Bully bootloader utility sends flash data for
 * PIC24FK devices in pages of 32 x 4 program words. 
 * Flash memory is programmed by rows of 32 program words.
 * 
 * Each instruction takes two memory addresses and
 * uses three bytes with top addressed byte unused.
 * Program data page that bootloader utility sends
 * uses 0x100 address locations and 128 instructions.
 */
#define PGM_ROW_COUNT   32
#define PGM_PAGE_COUNT  (PGM_ROW_COUNT * 4)
#define PGM_PAGE_BYTES  (PGM_PAGE_COUNT * 3)
#define PGM_ADDR_TOP    (__PROGRAM_BASE + __PROGRAM_LENGTH)
#define PGM_ROW_MASK    ((PGM_ROW_COUNT * 2) - 1)

#define EEM_PAGE_COUNT  16
#define EEM_PAGE_BYTES  (EEM_PAGE_COUNT * 2)
#define EEM_ADDR_TOP    (__EEDATA_BASE + __EEDATA_LENGTH)

/*
 * First page of General code segment starts at address 0xB00
 * where linker script will place branch instructions to user
 * interrupt functions as defined un user code. User code is
 * placed starting at address 0xC00.
 * 
 */
#define USER_CODE_START     0xC00

void __attribute__((boot)) write_buf_pgm(unsigned int, unsigned int);
void __attribute__((boot)) write_buf_eem(unsigned int, unsigned int);
