
// PIC24F08KA10x and PIC24F16KA10x Configuration Bit Settings

// FBS
#pragma config BWRP = ON                // Table Write Protect Boot (Boot segment is write-protected)
#pragma config BSS = STD1K              // Boot segment Protect (Standard security, boot program Flash segment starts at 200h, ends at 000AFEh)

// FGS
#pragma config GWRP = OFF               // General Segment Code Flash Write Protection bit (General segment may be written)
#pragma config GCP = OFF                // General Segment Code Flash Code Protection bit (No protection)

// FOSCSEL
#pragma config FNOSC = PRI              // Oscillator Select (Primary oscillator (XT, HS, EC))
#pragma config IESO = OFF               // Internal External Switch Over bit (Internal External Switchover mode disabled (Two-Speed Start-up disabled))

// FOSC
#pragma config POSCMOD = HS             // Primary Oscillator Configuration bits (HS oscillator mode selected)
#pragma config OSCIOFNC = ON            // CLKO Enable Configuration bit (CLKO output disabled; pin functions as port I/O)
#pragma config POSCFREQ = MS            // Primary Oscillator Frequency Range Configuration bits (Primary oscillator/external clock input frequency between 100 kHz and 8 MHz)
#pragma config SOSCSEL = SOSCLP         // SOSC Power Selection Configuration bits (Secondary oscillator configured for low-power operation)
#pragma config FCKSM = CSECME           // Clock Switching and Monitor Selection (Both Clock Switching and Fail-safe Clock Monitor are enabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscale Select bits (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (WDT prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Windowed Watchdog Timer Disable bit (Standard WDT selected; windowed WDT disabled)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))

// FPOR
#pragma config BOREN = BOR3             // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware; SBOREN bit disabled)
#pragma config PWRTEN = ON              // Power-up Timer Enable bit (PWRT enabled)
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Default location for SCL1/SDA1 pins)
#pragma config BORV = V27               // Brown-out Reset Voltage bits (Brown-out Reset set at 2.7V)
#pragma config MCLRE = ON               // MCLR Pin Enable bit (MCLR pin enabled; RA5 input pin disabled)

// FICD
#pragma config ICS = PGx3               // ICD Pin Placement Select bits (PGC3/PGD3 are used for programming and debugging the device)

// FDS
#pragma config DSWDTPS = DSWDTPSF       // Deep Sleep Watchdog Timer Postscale Select bits (1:2,147,483,648 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select bit (DSWDT uses LPRC as reference clock)
#pragma config RTCOSC = LPRC            // RTCC Reference Clock Select bit (RTCC uses LPRC as reference clock)
#pragma config DSBOREN = ON             // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR enabled in Deep Sleep)
#pragma config DSWDTEN = OFF            // Deep Sleep Watchdog Timer Enable bit (DSWDT disabled)
