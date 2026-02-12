#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

//#define BUFFER_SIZE 64
#define BUFFER_SIZE 4
#define PACKET_PAYLOAD_SIZE 256
#define MIN_TIME_BETWEEN_READS 150000
#define MAX_TIME_BETWEEN_READS 450000
#define TIME_BETWEEN_WRITES 300000

typedef struct {
    uint16_t length;
    uint16_t src_id;
    uint16_t dest_id;
    uint8_t payload[PACKET_PAYLOAD_SIZE];
    uint8_t crc;
} __attribute__((packed)) link_packet_t;
// Adding __attribute__((packed)) tells the compiler "No padding allowed." This ensures the sizeof(link_packet_t) is exactly 263

typedef struct {
    link_packet_t packets[BUFFER_SIZE];
    volatile uint8_t read_index;
    volatile uint8_t write_index;
    uint8_t mask;
    atomic_bool isFull;    
} ring_buffer_t;

void ring_buffer_init(ring_buffer_t *buf);
int ring_buffer_write_packet(ring_buffer_t *buf, const link_packet_t *packet);
int ring_buffer_read_packet(ring_buffer_t *buf, link_packet_t *packet);
int ring_buffer_is_full(ring_buffer_t *buf);
int ring_buffer_is_empty(ring_buffer_t *buf);

#endif // RING_BUFFER_H