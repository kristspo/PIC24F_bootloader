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

uint16_t count;
_TESTLOOP testState;
volatile uint16_t tick;

#define SET_BIT(x, mask) x |= mask
#define CLEAR_BIT(x, mask) x &= ~(mask)

int main(void) {
    // External 8 MHz oscillator is used after reset as clock source
    // Instruction clock is Fosc/4 at 4 MHz.

    // Setup Timer 1 for 0.5 s interrupt interval
    T1CONbits.TCKPS = 0b10; // 1:64 - 62.5 KHz
    PR1 = 31250;
    _T1IF = 0;
    _T1IE = 1;
    T1CONbits.TON = 1;
    // clear all pin output latches
    LATA = 0x0000;
    LATB = 0x0000;

    while (1) {
        while (count < (pincount << 2)) // four count 'ticks' for input pin
        {
            while (!tick) {
                __builtin_nop();
                __builtin_nop();
                __builtin_nop();
                __builtin_nop();
            }
            tick = 0;

            register uint16_t pin = (count >> 2);
            register uint16_t step = (count & 0x03);
            register uint16_t pinmask = 1 << (testpins[pin].pin_num);
            register uint16_t cnindex = testpins[pin].cnp_num;
            volatile uint16_t *reg;

            switch (testState) {
                case TEST_PUSH_PULL:
                case TEST_OPEN_DRAIN:
                    if (step == 0) {
                        // configure and set
                        if (testState == TEST_OPEN_DRAIN) {
                            reg = testpins[pin].port + 1; // LATx
                            CLEAR_BIT(*reg, pinmask);
                            reg = testpins[pin].port + 2; // ODCx
                            SET_BIT(*reg, pinmask);
                            ++count;
                        } else {
                            reg = testpins[pin].port + 1; // LATx
                            SET_BIT(*reg, pinmask);
                        }
                        reg = testpins[pin].port - 1; // TRISx
                        CLEAR_BIT(*reg, pinmask);
                    } else if (step == 1) {
                        // change state
                        reg = testpins[pin].port + 1; // LATx
                        CLEAR_BIT(*reg, pinmask);
                    } else if (step == 2) {
                        // clear configuration
                        reg = testpins[pin].port - 1; // TRISx
                        SET_BIT(*reg, pinmask);
                        if (testState == TEST_OPEN_DRAIN) {
                            reg = testpins[pin].port + 2; // ODCx
                            CLEAR_BIT(*reg, pinmask);
                        }
                        ++count;
                        tick = 1; // set next pin at same 'tick'
                    }
                    break;
                case TEST_PULL_DOWN:
                    if (step == 0) {
                        // configure and set
                        if (cnindex > 31) {
                            cnindex %= 32;
                            SET_BIT(CNPU3, (1 << cnindex));
                        } else if (cnindex > 15) {
                            cnindex %= 16;
                            SET_BIT(CNPU2, (1 << cnindex));
                        } else {
                            SET_BIT(CNPU1, (1 << cnindex));
                        }
                    } else if (step == 1) {
                        // change state
                        if (cnindex > 31) {
                            cnindex %= 32;
                            cnindex = (1 << cnindex);
                            CLEAR_BIT(CNPU3, cnindex);
                            __builtin_nop();
                            SET_BIT(CNPD3, cnindex);
                        } else if (cnindex > 15) {
                            cnindex %= 16;
                            cnindex = (1 << cnindex);
                            CLEAR_BIT(CNPU2, cnindex);
                            __builtin_nop();
                            SET_BIT(CNPD2, cnindex);
                        } else {
                            cnindex = (1 << cnindex);
                            CLEAR_BIT(CNPU1, cnindex);
                            __builtin_nop();
                            SET_BIT(CNPD1, cnindex);
                        }
                    } else if (step == 2) {
                        // clear configuration
                        if (cnindex > 31) {
                            cnindex %= 32;
                            CLEAR_BIT(CNPD3, (1 << cnindex));
                        } else if (cnindex > 15) {
                            cnindex %= 16;
                            CLEAR_BIT(CNPD2, (1 << cnindex));
                        } else {
                            CLEAR_BIT(CNPD1, (1 << cnindex));
                        }
                        ++count;
                        tick = 1; // set next pin at same 'tick'
                    }
                    break;
                case TEST_COUNT:
                    // avoid compiler warning
                    break;
            }
            ++count;
        }
        count = 0;
        testState = (testState + 1) % TEST_COUNT;
        __builtin_nop();
    }
    return 0;
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    _T1IF = 0;
    tick = 1;
}
