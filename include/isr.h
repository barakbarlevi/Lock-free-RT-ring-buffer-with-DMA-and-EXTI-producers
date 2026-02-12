#ifndef ISR_H
#define ISR_H

#include <stdint.h>
#include "ring_buffer.h"

#define DEBOUNCE_DELAY_TICKS 1  // 500ms (1 tick = 500ms from TIM2)

void packet_received_isr(link_packet_t * incoming_packet);
void* isr_simulator(void* arg);

#endif // ISR_H