#ifndef CRC_H
#define CRC_H

#include <stdint.h>
#include "ring_buffer.h"
#include "telemetry.h"

#define CHECKSUM_INVALID 0xFFFF
#define CRC8_POLYNOMIAL 0x07

uint8_t calculate_checksum(link_packet_t *pkt);
uint8_t crc8(const uint8_t* data, uint32_t length);

#endif // CRC_H