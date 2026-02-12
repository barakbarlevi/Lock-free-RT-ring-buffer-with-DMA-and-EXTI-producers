#!/usr/bin/env python3
import spidev
import time
import struct
import os

os.system('config-pin P9.17 spi_cs')
os.system('config-pin P9.18 spi')
os.system('config-pin P9.21 spi')
os.system('config-pin P9.22 spi_sclk')

spi = spidev.SpiDev()
spi.open(1, 0)
#spi.no_cs = True  # Add this line after spi.open()
spi.max_speed_hz = 1000000
spi.mode = 0

def calculate_crc8(data):
    """Simple CRC-8 calculation"""
    crc = 0
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x80:
                crc = (crc << 1) ^ 0x07
            else:
                crc <<= 1
            crc &= 0xFF
    return crc

def send_packet(val):
    src_id = 2      # Fixed: DMA source
    dest_id = val
    length = val
    
    # Pack header: length, src_id, dest_id (6 bytes total)
    header = struct.pack('<HHH', length, src_id, dest_id)
    
    # Payload: 256 bytes of 0xEE (distinct from button packets)
    payload = bytes([0xEE] * 256)
    
    # Calculate CRC over header + payload
    packet_data = header + payload
    crc = calculate_crc8(packet_data)
    
    # Send complete packet
    full_packet = packet_data + bytes([crc])
    
    # spi.xfer2(list(full_packet))
    # print(f"DMA packet sent: src=2, dest={dest_id}, len={length}, crc=0x{crc:02X}")
    result = spi.xfer2(list(full_packet))
    print(f"DMA packet sent: src=2, dest={dest_id}, len={length}, crc=0x{crc:02X}")
    #print(f"Received back (first 10 bytes): {result[:10]}")  # What STM32 sends on MISO

try:
    val = 100
    while True:
        send_packet(val)
        val += 100
        time.sleep(5)
except KeyboardInterrupt:
    print("\nStopped")
    spi.close()