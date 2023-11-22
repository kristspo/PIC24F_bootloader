
#include <stdint.h>

typedef struct {
    volatile uint16_t * port;
    uint16_t pin_num; // pin number
    uint16_t cnp_num; // input change configuration number
} _PINDEF;

typedef enum {
    TEST_PUSH_PULL,
    TEST_OPEN_DRAIN,
    TEST_PULL_DOWN,
    TEST_COUNT,
} _TESTLOOP;

/* pin configuration registers:
 * 0000 TRIS
 * 0002 PORT
 * 0004 LAT
 * 0006 ODC
 */

_PINDEF testpins[] = {
    {&PORTB, 15, 11},
    {&PORTB, 14, 12},
    {&PORTB, 13, 13},
    {&PORTB, 12, 14},
    {&PORTB, 9, 21},
    {&PORTB, 8, 22},
    {&PORTB, 7, 23},
    {&PORTB, 2, 6},
    {&PORTB, 1, 5},
    {&PORTB, 0, 4},

    {&PORTA, 0, 2},
    {&PORTA, 1, 3},
    // ISCP pins
    {&PORTB, 4, 1},
    {&PORTA, 4, 0},
    // status led
    {&PORTA, 6, 8},
    // oscillator pins
    // {&PORTA, 3, 29},
    // {&PORTA, 2, 30},
};

const uint16_t pincount = sizeof (testpins) / sizeof (_PINDEF);
