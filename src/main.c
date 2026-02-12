#include <stdio.h>
#include <stm32f446xx.h>
#include "uart.h"
#include "delay.h"
#include "timer.h"
#include "gpio.h"
#include "utils.h"
#include "ring_buffer.h"
#include "isr.h"
#include "spi_dma.h"

ring_buffer_t global_buffer;

extern volatile uint32_t timer_ticks; // From timer.c

int main(void) {

    // Initializations
    uart2_init();
    //gpio_init_led();
    timer2_init();
    user_button_init();
    ring_buffer_init(&global_buffer);
    spi2_dma_init();
    srand(time(NULL));  

    // Ran on host 
    //pthread_t isr_thread_write_packet;
    //pthread_create(&isr_thread_write_packet, NULL, isr_simulator, NULL);
      
    uint32_t counter = 0;
    uint32_t last_led_ticks = 0;
    uint32_t last_print_ticks = 0;
    link_packet_t packet;
    
    // Main loop
    while (1) {
        // LED toggle every 500ms (timer interrupt)
        if (timer_ticks != last_led_ticks) {
            //gpio_toggle_led();
            last_led_ticks = timer_ticks;
        }
        
        // Whatever's inside this if will run every ms_to_ticks(ms). Not that resolution of the time is currently set to 500[ms]
        if (timer_ticks - last_print_ticks >= ms_to_ticks(2000)) {
            
            // Poll for packets
            int read_packet_status = ring_buffer_read_packet(&global_buffer, &packet);
            if(read_packet_status == -1) {
                printf("Tried to read from an empty buffer\r\n");
            }

            if(read_packet_status == 1) {
                // Process the packet
                if(packet.src_id == 1) {
                    printf("Main loop read button interrupt packet: dest=%u, len=%u\r\n", packet.dest_id, packet.length);
                } else if(packet.src_id == 2) {
                    printf("Main loop read DMA packet: dest=%u, len=%u\r\n", packet.dest_id, packet.length);
                } else {
                    printf("Main loop read UNKNOWN packet: src=%u, dest=%u, len=%u\r\n", packet.src_id, packet.dest_id, packet.length);
                }
            }

            last_print_ticks = timer_ticks;
        }

    }
    
    return 0;
}
