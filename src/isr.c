#include "isr.h"
#include "ring_buffer.h"
#include "stm32f446xx.h"
#include <stdio.h>

extern ring_buffer_t global_buffer;
static uint16_t packet_counter = 1; // Packets to be inserted to the buffer are artificial 1, 2, 3,... in all fields
static uint32_t last_button_press_ticks = 0;
extern volatile uint32_t timer_ticks;


void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR & (1 << 13)) {  // Check if EXTI13 triggered
        EXTI->PR = (1 << 13);     // Clear pending flag

        // Debounce check
        if ((timer_ticks - last_button_press_ticks) < DEBOUNCE_DELAY_TICKS) {
            return;  // Ignore bounce
        }
        last_button_press_ticks = timer_ticks;
        
        // Create packet with incrementing values
        link_packet_t packet;
        packet.src_id = 1;
        packet.dest_id = packet_counter;
        packet.length = packet_counter;
        ring_buffer_write_packet(&global_buffer, &packet);
        packet_counter++;
    }
}



/**
 * @brief simulates the ISR that's called when DMA completes
 */
void packet_received_isr(link_packet_t * incoming_packet) {
    // link_packet_t incoming_packet;
    // incoming_packet.length = length;
    // incoming_packet.src_id = src_id;
    // incoming_packet.dest_id = dest_id;
    buffer_write_packet(&global_buffer, incoming_packet);
    printf("ISR wrote the packet: src=%u, dest=%u, len=%u\r\n", incoming_packet->src_id, incoming_packet->dest_id, incoming_packet->length);
}

void* isr_simulator(void* arg) {
    uint16_t length = 0;
    uint16_t src_id = 0;
    uint16_t dest_id = 0;

    while(1) {
        usleep(TIME_BETWEEN_WRITES);
        link_packet_t local_packet;
        local_packet.length = length;
        local_packet.src_id = src_id;
        local_packet.dest_id = dest_id;
        packet_received_isr(&local_packet);

        length++;
        src_id++;
        dest_id++;
    }
    return NULL;
}