#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

#define LED_PORT GPIOA
#define LED_PIN 5

void gpio_init_led(void);
void gpio_toggle_led(void);

#endif // GPIO_H