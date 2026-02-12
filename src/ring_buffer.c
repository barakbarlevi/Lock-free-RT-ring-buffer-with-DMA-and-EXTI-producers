#include "ring_buffer.h"


void ring_buffer_init(ring_buffer_t *buf) {
    buf->mask = BUFFER_SIZE - 1;
    buf->read_index = 0;
    buf->write_index = 0;
    
    for(uint8_t i = 0; i < BUFFER_SIZE; i++) {
        buf->packets[i].dest_id = 0;
        buf->packets[i].length = 0;
        buf->packets[i].src_id = 0;
        for(uint16_t j = 0; j < PACKET_PAYLOAD_SIZE; j++) {
            buf->packets[i].payload[j] = 0;
        }
    }
}

int ring_buffer_write_packet(ring_buffer_t *buf, const link_packet_t *packet) {
    
    // Atomic load of isFull
    if(atomic_load(&buf->isFull)) {
        // Buffer full - Losing data (overriding oldest). The read pointer advances, overriding the oldest packet
        buf->read_index = (buf->read_index + 1)& buf->mask;
        printf("Overriden data when wrote the packet: src=%u, dest=%u, len=%u\r\n", packet->src_id, packet->dest_id, packet->length);
    }

    buf->packets[buf->write_index] = *packet;

    // In a modern high-performance CPU, the processor is allowed to reorder instructions to keep its internal pipelines full.
    // regarding the two lines right above and below the memory barrier:
    // The CPU sees that these two memory locations are unrelated (they have different addresses).
    // To save time, the CPU hardware might decide to send the "Update index" command to the RAM controller before the "Write data" command is finished.
    __sync_synchronize();  // Memory barrier - ensure packet write completes before updating the index. make sure the Packet Data hits the RAM before the Index Update hits the RAM.
    // A Memory Barrier acts as a "Wall." It tells the CPU: "Do not let any memory writes from before this wall cross over to after this wall."
    buf->write_index = (buf->write_index + 1) & buf->mask;

    if(buf->write_index == buf->read_index) {
        atomic_store(&buf->isFull, true); // Atomic set when full
    } 
    return 0;
}

int ring_buffer_read_packet(ring_buffer_t *buf, link_packet_t *packet) {
    if(ring_buffer_is_empty(buf)) return -1;

    __sync_synchronize();  // Memory barrier - ensure we see latest packet data
    // Without the barrier: CPU/compiler might reorder to:
    // Read packet data first (speculatively)
    // Then check if empty
    // Result: We read stale/garbage data before confirming buffer has valid data

    *packet = buf->packets[buf->read_index];
    buf->read_index = (buf->read_index + 1) & buf->mask;
    atomic_store(&buf->isFull, false);  // Atomic clear after consuming

    return 1;
}

int ring_buffer_is_full(ring_buffer_t *buf) {
    return atomic_load(&buf->isFull);  // Atomic read
}

int ring_buffer_is_empty(ring_buffer_t *buf) {
    return (!atomic_load(&buf->isFull) && (buf->read_index == buf->write_index));
}
