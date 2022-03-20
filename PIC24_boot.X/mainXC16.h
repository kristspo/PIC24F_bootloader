
#define COMMAND_NACK        0x00
#define COMMAND_ACK         0x01
#define COMMAND_READ_PM     0x02
#define COMMAND_WRITE_PM    0x03
#define COMMAND_READ_EM     0x04 /* not used in original Bully bootloader utility */
#define COMMAND_WRITE_EM    0x05 /* used in original Bully bootloader utility but not documented  */
#define COMMAND_WRITE_CM    0x07
#define COMMAND_CM_RESET    0x08 /* used after COMMAND_WRITE_CM in original Bully bootloader utility */
#define COMMAND_READ_ID     0x09
#define COMMAND_READ_VERS   0x11
#define COMMAND_RESET       0x13
#define COMMAND_IDLE        0xFFFF

#define VERSION_MAJOR 0x03
#define VERSION_MINOR 0x0A

typedef union t_reg16 {
    unsigned int w;
    struct {
        unsigned char b0; // low byte
        unsigned char b1;
    };
} reg16;

typedef union t_reg32 {
    unsigned long w32;
    struct {
        unsigned int lw; // low word
        unsigned int hw;
    };
} reg32;

/* Locate bootloader functions in boot code segment */

int __attribute__((boot)) main();
void __attribute__((boot)) cleanExit();

unsigned int __attribute__((boot)) readChar();
unsigned int __attribute__((boot)) waitChar();
void __attribute__((boot)) sendWord(reg16);
void __attribute__((boot)) sendChar(unsigned char);
