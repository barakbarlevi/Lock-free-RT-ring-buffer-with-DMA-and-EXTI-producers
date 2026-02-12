#include "crc.h"


uint8_t calculate_checksum(link_packet_t *pkt) {
    
    // Check for invalid packet length
    if (pkt->length > PACKET_PAYLOAD_SIZE) {
        // Log the error in g_stats
        g_stats.malformed_packets++; 
        return CHECKSUM_INVALID; 
    }

    uint32_t sum = 0;
    
    // Sum up the header fields
    sum += pkt->length;
    sum += pkt->src_id;
    sum += pkt->dest_id;

    // Sum up the payload
    // PACKET_PAYLOAD_SIZE is the maximum possible size of the buffer. However, a specific packet might only be carrying less bytes of actual data
    // The rest might be uninitialized garbage
    for (int i = 0; i < pkt->length; i++) {
        sum += pkt->payload[i];
    }

    // Return the lower 8 bits as the checksum
    // why only the 8 lower bits? Every bit sent costs power and time. If you can get "good enough" error detection with 8 bits, you don't waste 32 bits on every single packet. Protocol dependent
    return (uint8_t)(sum & 0xFF);
}


uint8_t crc8(const uint8_t *data, uint32_t len) {
    uint8_t crc = 0x00;

    for (uint32_t i = 0; i < len; i++) {
        crc ^= data[i];

        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            } else {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}