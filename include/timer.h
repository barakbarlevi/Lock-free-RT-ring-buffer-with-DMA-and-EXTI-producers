#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// --- Hardware Constants ---
#define SYS_CLK_HZ              16000000UL  // 16 MHz HSI Clock
#define APB1_PRESCALER          1           // Default on reset
#define TIMER_CLK_HZ            (SYS_CLK_HZ / APB1_PRESCALER)

// --- Application Settings ---
#define TIMER_RESOLUTION_MS     500         // How often the timer interrupt actually fires
#define POLL_INTERVAL_MS        2000        // The desired polling interval

// --- Register Math ---
#define PRESCALER_VALUE         (TIMER_CLK_HZ / 1000) // We want 1ms internal resolution for the counter (1kHz)
#define AUTO_RELOAD_VALUE       TIMER_RESOLUTION_MS

// The calculation for the comparison
#define POLL_INTERVAL_TICKS     (POLL_INTERVAL_MS / TIMER_RESOLUTION_MS)

static inline uint32_t ms_to_ticks(uint32_t ms) {
    // Standard trick to round up integer division: (numerator + denominator - 1) / denominator
    return (ms + TIMER_RESOLUTION_MS - 1) / TIMER_RESOLUTION_MS;
}

void timer2_init(void);

#endif // TIMER_H