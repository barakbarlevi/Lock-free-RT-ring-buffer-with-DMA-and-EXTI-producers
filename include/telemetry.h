#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdint.h>

typedef struct {
    uint64_t total_packets_received;
    uint64_t crc_errors;           // Corrupted on wire
    uint64_t buffer_overruns;      
    uint64_t total_bytes_processed;
    uint64_t malformed_packets;
} telemetry_stats_t;

// Global instance (usually initialized to 0)
telemetry_stats_t g_stats = {0};

#endif // TELEMETRY_H