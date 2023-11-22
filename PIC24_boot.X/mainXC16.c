/*
 * Copyright (c) 2023 KPT
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "xc.h"

#include "mainXC16.h"
#include "flash.h"

unsigned char __attribute__((persistent)) pgm_buff[PGM_PAGE_BYTES];
unsigned char __attribute__((persistent)) unused[512 - PGM_PAGE_BYTES];

int main() {

    // RA6 on board led set output high while at bootloader
    LATAbits.LATA6 = 1;
    TRISAbits.TRISA6 = 0;
    // RB7 UART1 TX output preset high
    LATBbits.LATB7 = 1;
    TRISBbits.TRISB7 = 0;
#ifdef ANSB
    // RB2 UART1 RX disable analog input
    ANSBbits.ANSB2 = 0;
#else
    /* Devices with 10 bit ADC configure pin analog function in AD1PCFG register.
     * 28 pin PIC24FxxKA102 has AN4 channel on RB2 pin that needs to be disabled.
     * 20 pin PIC24FxxKA101 does not have analog function on RB2 pin.
     */
#if defined(__PIC24F08KA102__) || defined(__PIC24F16KA102__)
    AD1PCFGbits.PCFG4 = 1;
#else
    __builtin_nop();
#endif
#endif

    // Setup UART1 for 38400 baud rate
    // BRG value with BRGH set is: (4 000 000 / (4 * baudrate)) - 1
    U1BRG = 25; // +0.16% error in actual baud rate
    U1MODE = 0x8008; // 8 bit no parity one stop bit; BRGH bit set
    
    // Setup Timer2 for 0.5 seconds period to exit bootloader
    T2CON = 0x0020; // Timer clock 1:64 Fosc/2 (62.5 kHz)
    PR2 = 31250;
    _T2IF = 0;
    T2CONbits.TON = 1;

    register unsigned int z;
    reg32 addr;
    
    while (1) {
        switch (readChar()) {

            case COMMAND_WRITE_PM: /* write FLASH  */
                z = waitChar(); // LSB of address
                addr.lw = z | (waitChar() << 8);
                addr.hw = waitChar();
                z = PGM_PAGE_BYTES;
                while (z) {
                    pgm_buff[--z] = waitChar();
                }
                if (addr.w32 <= (PGM_ADDR_TOP - (PGM_PAGE_COUNT * 2))) {
                    TBLPAG = addr.hw;
                    write_buf_pgm(addr.lw, PGM_PAGE_BYTES);
                }
                sendChar(COMMAND_ACK);
                break;

            case COMMAND_WRITE_EM: /* write EEPROM */
                z = waitChar(); // LSB of address
                addr.lw = z | (waitChar() << 8);
                addr.hw = waitChar();
                z = EEM_PAGE_BYTES;
                while (z) {
                    pgm_buff[--z] = waitChar();
                }
                if (addr.w32 <= (EEM_ADDR_TOP - (EEM_PAGE_COUNT * 2))) {
                    TBLPAG = addr.hw;
                    write_buf_eem(addr.lw, EEM_PAGE_BYTES);
                }
                sendChar(COMMAND_ACK);
                break;

            case COMMAND_READ_PM: /* read FLASH  */
                z = waitChar(); // LSB of address
                addr.lw = z | (waitChar() << 8);
                addr.hw = waitChar();
                if (addr.w32 <= (PGM_ADDR_TOP - (PGM_PAGE_COUNT * 2))) {
                    TBLPAG = addr.hw;
                    for (z = PGM_PAGE_COUNT; z; z--) {
                        addr.hw = __builtin_tblrdh(addr.lw);
                        sendChar(addr.hw); // MSB
                        addr.hw = __builtin_tblrdl(addr.lw);
                        sendChar(addr.hw >> 8);
                        sendChar(addr.hw); // LSB
                        // update address to read
                        addr.lw += 2;
                    }
                } else {
                    for (z = PGM_PAGE_BYTES; z; z--) {
                        sendChar(0xFF);
                    }
                }
                break;

            case COMMAND_READ_EM: /* read EEPROM  */
                z = waitChar(); // LSB of address
                addr.lw = z | (waitChar() << 8);
                addr.hw = waitChar();
                if (addr.w32 <= (EEM_ADDR_TOP - (EEM_PAGE_COUNT * 2))) {
                    TBLPAG = addr.hw;
                    for (z = EEM_PAGE_COUNT; z; z--) {
                        addr.hw = __builtin_tblrdl(addr.lw);
                        sendChar(addr.hw >> 8);
                        sendChar(addr.hw);
                        // update address to read
                        addr.lw += 2;
                    }
                } else {
                    for (z = EEM_PAGE_BYTES; z; z--) {
                        sendChar(0xFF);
                    }
                }
                break;
                
            /*
             * COMMAND_READ_ID or COMMAND_READ_VERS are initial commands
             * received from bootloader utility to start update operation
             */

            case COMMAND_READ_ID: /* read device id  */
                // PIC24F16KA301 : 0x4508 0x0007
                // PIC24F16KA301 : 0x4518 0x000?
                // PIC24F16KA302 : 0x4502 0x000?
                // PIC24F32KA302 : 0x4512 0x0007
                T2CONbits.TON = 0; // stop timeout timer
                TBLPAG = 0xFF;
                addr.lw = __builtin_tblrdl(0);
                addr.hw = __builtin_tblrdl(2);
                if (!U1STAbits.UTXEN) {
                    U1STAbits.UTXEN = 1;
                }
                sendWord((reg16) addr.lw);
                sendWord((reg16) 0x0000u);
                sendWord((reg16) addr.hw);
                sendWord((reg16) 0x0000u);
                break;

            case COMMAND_READ_VERS: /* read bootloader version  */
                T2CONbits.TON = 0; // stop timeout timer
                if (!U1STAbits.UTXEN) {
                    U1STAbits.UTXEN = 1;
                }
                sendChar(VERSION_MAJOR);
                sendChar(VERSION_MINOR);
                sendChar(COMMAND_ACK);
                break;

            case COMMAND_RESET: /* reset to user application */
                __asm__ volatile ("reset");
                break;

            case COMMAND_IDLE:
                // check if boot timeout has elapsed
                if (T2CONbits.TON && _T2IF) {
                    T2CONbits.TON = 0; // stop timer
                    // check if there is instruction at flash memory address where user code starts
                    TBLPAG = 0x00;
                    addr.lw = __builtin_tblrdl(USER_CODE_START);
                    addr.hw = __builtin_tblrdh(USER_CODE_START);
                    if (addr.w32 != 0x00FFFFFF) {
                        // redirect to user code
                        cleanExit();
                    }
                    // will stay in bootloader if user code is not detected
                }
                break;

            default:  /* unknown command or unexpected transmission */
                __builtin_nop();
        }
    }
    return 0;
}

/* Exit bootloader - reset changed registers and pass control to user application */

void cleanExit() {
    // clear on board led
    LATAbits.LATA6 = 0;
    TRISAbits.TRISA6 = 1;
    // disable UART1 (U1BRG register is not reset)
    U1MODE = 0;
    U1STA = 0;
    // reset UART1 TX pin direction
    TRISBbits.TRISB7 = 1;
    // output latch of UART1 TX pin remains high
    // analog buffer on UART1 RX pin remains disabled
    // reset Timer2
    T2CON = 0;
    _T2IF = 0;
    TMR2 = 0;
    PR2 = 0xFFFF;

    asm("goto %0" : : "i"(USER_CODE_START));
}

/* UART receive and send functions */

unsigned int readChar() {
	// check if character is received over UART
    if (U1STAbits.FERR) {
        __builtin_nop();
        __asm__ volatile ("reset");
    }
    if (U1STAbits.OERR) {
        __builtin_nop();
        __asm__ volatile ("reset");
    }
    if (U1STAbits.URXDA) {
        return U1RXREG;
    }
    return COMMAND_IDLE;
}

unsigned int waitChar() {
    while (1) {
        // wait until character is received over UART
        if (U1STAbits.URXDA) {
            return U1RXREG;
        }
    }
}

void sendWord(reg16 ch) {
    sendChar(ch.b0);
    sendChar(ch.b1);
}

void sendChar(unsigned char ch) {
    while (U1STAbits.UTXBF) {
        __builtin_nop();
    }
    U1TXREG = ch;
}
