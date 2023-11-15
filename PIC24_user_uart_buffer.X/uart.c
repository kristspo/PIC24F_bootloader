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

#include <xc.h>
#include "uart.h"

// Use if bootloader was use to set up some UART settings
#define BOOTLOADER_SETUP
// RX and TX buffer size
#define UART_RX_LENGTH 60
#define UART_TX_LENGTH 256 // Value that is power of two uses less instruction cycles

char rx_buffer[UART_RX_LENGTH];
char tx_buffer[UART_TX_LENGTH];

uint16_t rx_count; // Characters received in rx_buffer
uint16_t tx_count; // Characters to transmit from tx_buffer
uint16_t tx_get;   // Read index of tx_buffer
uint16_t tx_put;   // Write index of tx_buffer

uint16_t rx_cmd_length;

/*
 * Setup UART1:
 * Set RB7 UART1 TX pin as output.
 * Analog input of RB2 UART1 RX pin is already disabled by bootloader.
 * BRG value is already set by bootloader for 38400 baud rate and BRGH bit set.
 *
 * If code is uploaded using ICSP programmer instead of bootloader, all settings
 * need to be set for UART1 to function. Uncomment #define BOOTLOADER_SETUP to do so.
 */
void uart_init() {

    TRISBbits.TRISB7 = 0;
#ifndef BOOTLOADER_SETUP
    ANSBbits.ANSB2 = 0;
    U1BRG = 25;
#endif
    // Enable UART with 8 bit no parity one stop bit, BRGH bit set
    U1MODE = _U1MODE_UARTEN_MASK | _U1MODE_BRGH_MASK;
    // Use 0b00 for UTXISEL and URXISEL transmit and receive interrupt mode
    U1STA = _U1STA_UTXEN_MASK;
    /* TX (buffer empty) interrupt enabling UART transmitter.
     * Enable TX interrupt when there are characters to transmit.
     */
    // Enable UART RX and ERR interrupt
    IEC0bits.U1RXIE = 1;
    IEC4bits.U1ERIE = 1;
}

/*
 * Send null terminated character string adding new line characters
 */
void uart_send_str(const char * str) {
    // Check length of null terminated string
    register uint16_t length = 0;
    while (length < (UART_TX_LENGTH - 2) && str[length] != 0) {
        ++length;
    }

    // Wait while there is enough free space in TX buffer.
    while ((tx_count + length + 2) > UART_TX_LENGTH) {
        __builtin_nop();
        __builtin_nop();
        __builtin_nop();
        __builtin_nop();
    }

    // Disable TX interrupt while updating TX buffer
    IEC0bits.U1TXIE = 0;

    register uint16_t i = tx_put;
    register uint16_t count = 0;
    while (count < length) {
        tx_buffer[i] = *str++;
        i = (i + 1) % UART_TX_LENGTH;
        ++count;
    }
    tx_buffer[i] = '\r';
    i = (i + 1) % UART_TX_LENGTH;
    tx_buffer[i] = '\n';
    i = (i + 1) % UART_TX_LENGTH;
    // Save update TX buffer counters
    tx_put = i;
    tx_count += (count + 2);

    // Enable TX interrupt
    IEC0bits.U1TXIE = 1;
}

/*
 * Send character buffer optionally adding new line characters
 */
void uart_send_buf(const char * buf, register uint16_t length, register bool newline) {
    // Check length of buffer to transmit
    if (length > UART_TX_LENGTH) length = UART_TX_LENGTH;

    // Wait while there is enough free space in TX buffer
    while ((tx_count + length + (newline ? 2 : 0)) > UART_TX_LENGTH) {
        __builtin_nop();
        __builtin_nop();
        __builtin_nop();
        __builtin_nop();
    }

    // Disable TX interrupt while updating TX buffer
    IEC0bits.U1TXIE = 0;

    register uint16_t i = tx_put;
    register uint16_t count = 0;
    while (count < length) {
        tx_buffer[i] = *buf++;
        i = (i + 1) % UART_TX_LENGTH;
        ++count;
    }
    if (newline) {
        tx_buffer[i] = '\r';
        i = (i + 1) % UART_TX_LENGTH;
        tx_buffer[i] = '\n';
        i = (i + 1) % UART_TX_LENGTH;
        count += 2;
    }
    // Save update TX buffer counters
    tx_put = i;
    tx_count += count;

    // Enable TX interrupt
    IEC0bits.U1TXIE = 1;
}

void uart_restart_rx() {
    rx_count = 0;
    // Enable UART RX interrupt;
    IEC0bits.U1RXIE = 1;
}

void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0;

    do {
        char ch = U1RXREG;
        if (ch == '\r' || ch == '\n') {
            if (rx_count) {
                // Put new line in TX buffer
                tx_buffer[tx_put] = '\r';
                tx_put = (tx_put + 1) % UART_TX_LENGTH;
                tx_buffer[tx_put] = '\n';
                tx_put = (tx_put + 1) % UART_TX_LENGTH;
                tx_count += 2;
                // Enable TX interrupt
                IEC0bits.U1TXIE = 1;
                // Save received character count for processing
                rx_cmd_length = rx_count;
                // Disable UART RX interrupt until line will be processed
                IEC0bits.U1RXIE = 0;
            } else {
                __builtin_nop();
            }
        } else {
            if (rx_count < UART_RX_LENGTH) {
                rx_buffer[rx_count] = ch;
                rx_count++;
            }
            // Put received character in TX buffer
            tx_buffer[tx_put] = ch;
            tx_put = (tx_put + 1) % UART_TX_LENGTH;
            tx_count++;
            // Enable TX interrupt
            IEC0bits.U1TXIE = 1;
        }
    } while (U1STAbits.URXDA); // get all characters from UART RX buffer
}

void __attribute__((interrupt, auto_psv)) _U1TXInterrupt(void) {
    if (tx_count) {
        // Clear TX interrupt flag and transmit next character
        IFS0bits.U1TXIF = 0;
        U1TXREG = tx_buffer[tx_get];
        tx_get = (tx_get + 1) % UART_TX_LENGTH;
        --tx_count;
    } else {
        // Disable TX interrupt but leave interrupt flag set
        IEC0bits.U1TXIE = 0;
    }
}

void __attribute__((interrupt, auto_psv)) _U1ErrInterrupt(void) {
    IFS4bits.U1ERIF = 0;
    // Clear receive buffer overrun error bit to allow further reception
    if (U1STAbits.OERR) {
        U1STAbits.OERR = 0;
    }
}
