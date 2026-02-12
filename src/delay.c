#include "delay.h"

// Simple busy-wait delay
// Assumes 16 MHz HSI clock (default after reset)
// Very rough approximation
void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        // Roughly 1ms at 16 MHz
        for (volatile uint32_t j = 0; j < 1400; j++);
    }
}
