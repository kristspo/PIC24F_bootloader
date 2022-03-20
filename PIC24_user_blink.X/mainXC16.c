
#include "config.h"
#include "xc.h"

int main(void) {
    // External 8 MHz oscillator is used after reset as clock source
    // Instruction clock is Fosc/4 at 4 MHz.

    // RA6 pin on board led
    LATAbits.LATA6 = 0;
    TRISAbits.TRISA6 = 0;
    
    // Setup Timer 1 for 0.5 s interrupt interval
    T1CONbits.TCKPS = 0b10; // 1:64 - 62.5 KHz
    PR1 = 31250;
    _T1IF = 0;
    _T1IE = 1;
    T1CONbits.TON = 1;

    while (1) {
        __builtin_nop();
    }
    return 0;
}

void _ISR _T1Interrupt(void) {
    _T1IF = 0;
    LATAbits.LATA6 ^= 1; // toggle led
}
