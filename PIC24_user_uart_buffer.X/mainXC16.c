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
#include "uart.h"

// Set when line is received over UART, UART RX interrupt is disabled
extern uint16_t rx_cmd_length;
// Read UART RX buffer directly to save changed input string
extern char rx_buffer[];

int main(void) {
    /* External 8 MHz oscillator is used after reset as clock source.
     * Instruction clock for PIC24 is Fosc/2 (4 Mhz).
     * Watchdog timer is disabled by configuration fuse.
     */

    // RA6 pin on board led
    LATAbits.LATA6 = 0;
    TRISAbits.TRISA6 = 0;

    uart_init();
    uart_send_str("Hello!");
    uart_send_str("Enter line to convert:");
    // Try to send long string that will be trimmed to TX buffer size
    /*
    uart_send_str("CbL3cEKDMeKk745FGcZjtaxQY4RtTYiq\
tYB8RS8U5bSjLfOVtcUiD8JUnlVcriL6\
9k4aOMIZLRsYAcMYm4MI53E4EGh4qTwM\
f1o8OqZtqCKE7LBP207E7ntNwLlEvyO1\
kFmDcscS3HNIDJ3odLByKWPuCu3QmWIn\
oepz43BaJoRY7JO7BFjbRIpU3ffpxB30\
WFRG4RrJi5NPZAivJDQqeixfDmsuYgHZ\
MOyMLvsK9ghwuV38yeC1Zk0IacX9gFZq\
zmesJ7cPswcYKdHssHpSBPUBDwRftoAC\
3u9Fr2hxJkuRqpLeZywJjJOF6fex4clm");
     */

    while (1) {
        if (rx_cmd_length) {
            register uint16_t tx_length = 0;
            for (register uint16_t z = 0; z < rx_cmd_length; z++) {
                register char ch = rx_buffer[z];
                if (ch < 0x20 || (ch > 0x7E)) continue; // Ignore unprintable characters
                if (ch >= 'a' && ch <= 'z') ch -= 32; // Convert lower case characters
                // Reuse RX buffer to save converted string
                rx_buffer[tx_length] = ch;
                ++tx_length;
            }
            if (tx_length) {
                /* Send buffer adding newline.
                 * Sending will block if there is not enough space in TX buffer.
                 * In this case incoming characters can be lost. This can be
                 * avoided by using another buffer for converted string so that
                 * RX buffer can be used for incoming characters right away.
                 */
                uart_send_buf(rx_buffer, tx_length, true);
            }
            rx_cmd_length = 0;
            uart_restart_rx();
        }
    }
}
