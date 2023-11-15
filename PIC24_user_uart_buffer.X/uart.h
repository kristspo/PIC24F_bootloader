
#include <stdint.h>
#include <stdbool.h>

#ifndef XC_UART_H
#define	XC_UART_H

void uart_init();
void uart_send_str(const char *);
void uart_send_buf(const char *, uint16_t, bool);
void uart_restart_rx();

#endif	/* XC_UART_H */