#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart2_init(void);
void uart2_write(uint8_t ch);
void uart2_write_string(const char *str);

#endif // UART_H
