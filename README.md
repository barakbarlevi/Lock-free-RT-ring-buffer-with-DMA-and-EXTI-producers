# Lock free ring buffer for real time packet processing using DMA and EXTI producers

Bare metal embedded firmware demonstration on an STM32-NucleoF446RE of a lock free ring buffer. Implements multi-producer synchronization using atomic operations and DMA-based packet reception over SPI. Memory barriers prevent CPU instruction reordering.
![Image](https://github.com/user-attachments/assets/7867b839-08c8-4d98-adb8-70527b393e2f)

#### Packet Flow
1. **Source 1 (Button):** Press blue button → EXTI interrupt → writes packet with `src_id=1`
2. **Source 2 (DMA/SPI):** BBB sends packet over SPI → DMA copies to staging buffer → interrupt → writes packet with `src_id=2`
3. **Consumer:** Main loop polls ring buffer every 2 seconds, processes packets

#### Synchronization

`isFull` flag accessed by both ISRs. Thus delared `atomic bool`, and `main()`  uses `atomic_load()`/`atomic_store()`


#### Build and flash
```bash
make clean
make
make flash  

picocom /dev/ttyACM0 -b 115200
```

---

## BeagleBone Black Setup
| BBB | STM32 Nucleo-F446RE | Signal |
|------------------|--------------|--------|
| P9.18            | PA7          | MOSI   |
| P9.21            | PA6          | MISO   |
| P9.22            | PA5          | SCK    |
| P9.1 (GND)       | GND          | Ground |

NSS is software configured.

#### Install Dependencies
```bash
sudo apt-get update
git clone https://github.com/doceme/py-spidev.git
cd py-spidev
sudo python3 setup.py install
```

#### Configure SPI Pins
```bash
config-pin P9.17 spi_cs
config-pin P9.18 spi
config-pin P9.21 spi
config-pin P9.22 spi_sclk
```

#### Run Packet Sender
```bash
python3 beaglebone_spi.py
```

**Expected output:**
```
DMA packet sent: src=2, dest=100, len=100, crc=0xB0
DMA packet sent: src=2, dest=200, len=200, crc=0xA2
...
```

---

## Example Output on host
```
Terminal ready
Tried to read from an empty buffer
Main loop read BUTTON packet: dest=1, len=1
Tried to read from an empty buffer
Main loop read DMA packet: dest=100, len=100
Main loop read DMA packet: dest=200, len=200
Tried to read from an empty buffer
Main loop read BUTTON packet: dest=2, len=2
```
