#include "spi_dma.h"
#include "stm32f446xx.h"
#include <stdio.h>

// Staging buffer for DMA to write into
static link_packet_t dma_staging_buffer __attribute__((aligned(4)));

extern ring_buffer_t global_buffer;

void spi2_dma_init(void) {
    // 1. Enable clocks
    //RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;   // GPIOB for SPI2 pins
    //RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;    // SPI2
    //RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;    // DMA1

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;   // GPIOA for SPI1 pins
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;    // SPI1

    // RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;    // SPI1
    // RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;  // Reset SPI1
    // RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST; // Release reset

    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;    // DMA2 (not DMA1!)
    
    // 2. Configure GPIO pins for SPI2
    // PB12=NSS, PB13=SCK, PB14=MISO, PB15=MOSI
    //GPIOB->MODER &= ~((3<<24) | (3<<26) | (3<<28) | (3<<30));
    //GPIOB->MODER |=  ((2<<24) | (2<<26) | (2<<28) | (2<<30)); // Alternate function

    // 2. Configure GPIO pins for SPI1
    // PA4=NSS, PA5=SCK, PA6=MISO, PA7=MOSI
    GPIOA->MODER &= ~((3<<8) | (3<<10) | (3<<12) | (3<<14));
    GPIOA->MODER |=  ((2<<8) | (2<<10) | (2<<12) | (2<<14)); // Alternate function

    
    // Set alternate function AF5 (SPI2) for all pins
    //GPIOB->AFR[1] &= ~((0xF<<16) | (0xF<<20) | (0xF<<24) | (0xF<<28));
    //GPIOB->AFR[1] |=  ((5<<16) | (5<<20) | (5<<24) | (5<<28));

    // Set alternate function AF5 (SPI1) for all pins
    GPIOA->AFR[0] &= ~((0xF<<16) | (0xF<<20) | (0xF<<24) | (0xF<<28));
    GPIOA->AFR[0] |=  ((5<<16) | (5<<20) | (5<<24) | (5<<28));
    
    // 3. Configure SPI2 as slave (don't enable yet)
    //SPI2->CR1 = 0; // Reset - ensures MSTR bit is 0 (slave mode)
    //SPI2->CR1 &= ~SPI_CR1_MSTR;  // Slave mode
    //SPI2->CR1 &= ~SPI_CR1_CPOL;  // Clock polarity low when idle
    //SPI2->CR1 &= ~SPI_CR1_CPHA;  // First clock transition is first data capture
    // CPOL=0, CPHA=0 (mode 0) by default

    // 3. Configure SPI1 as slave
    SPI1->CR1 = 0; // Reset
    SPI1->CR1 &= ~SPI_CR1_MSTR;  // Slave mode
    SPI1->CR1 &= ~SPI_CR1_CPOL;  // Clock polarity low when idle
    SPI1->CR1 &= ~SPI_CR1_CPHA;  // First clock transition is first data capture

    // 3b. Configure NSS (slave select) - software management
    //SPI2->CR1 |= SPI_CR1_SSM;  // Software slave management // didn't work, commenting out
    //SPI2->CR1 |= SPI_CR1_SSI;  // Internal slave select // didn't work, commenting out
    // 3b. Configure CR2 for hardware NSS
    //SPI2->CR2 = 0;
    //SPI2->CR2 &= ~SPI_CR2_SSOE;  // NSS output disable (slave mode)

    // 3b. Configure CR2 for hardware NSS
    SPI1->CR2 = 0;
    SPI1->CR2 &= ~SPI_CR2_SSOE;  // NSS output disable (slave mode)
    
    // 4. Configure DMA1 Stream 3 (SPI2_RX)
    // DMA1_Stream3->CR = 0; // Reset
    // while(DMA1_Stream3->CR & DMA_SxCR_EN); // Wait until disabled
    // // Channel 0 = SPI2_RX
    // DMA1_Stream3->CR |= (0 << DMA_SxCR_CHSEL_Pos);
    // // Memory increment, peripheral-to-memory, 8-bit transfers
    // DMA1_Stream3->CR |= DMA_SxCR_MINC;                     // Increment memory address
    // DMA1_Stream3->CR |= (0 << DMA_SxCR_MSIZE_Pos);        // 8-bit memory
    // DMA1_Stream3->CR |= (0 << DMA_SxCR_PSIZE_Pos);        // 8-bit peripheral
    // DMA1_Stream3->CR |= (1 << DMA_SxCR_PL_Pos);           // Medium priority
    // // Addresses
    // DMA1_Stream3->PAR = (uint32_t)&(SPI2->DR);            // Peripheral address
    // DMA1_Stream3->M0AR = (uint32_t)&dma_staging_buffer;   // Memory address
    // DMA1_Stream3->NDTR = sizeof(link_packet_t);         // 263 bytes
    // // Enable transfer complete interrupt
    // DMA1_Stream3->CR |= DMA_SxCR_TCIE;


     // 4. Configure DMA2 Stream 0 (SPI1_RX, Channel 3)
    DMA2_Stream0->CR = 0; // Reset
    while(DMA2_Stream0->CR & DMA_SxCR_EN); // Wait until disabled
    
    // Channel 3 = SPI1_RX
    DMA2_Stream0->CR |= (3 << DMA_SxCR_CHSEL_Pos);
    
    // Memory increment, peripheral-to-memory, 8-bit transfers
    DMA2_Stream0->CR |= DMA_SxCR_MINC;                     // Increment memory address
    DMA2_Stream0->CR |= (0 << DMA_SxCR_MSIZE_Pos);        // 8-bit memory
    DMA2_Stream0->CR |= (0 << DMA_SxCR_PSIZE_Pos);        // 8-bit peripheral
    DMA2_Stream0->CR |= (1 << DMA_SxCR_PL_Pos);           // Medium priority
    
    // Addresses
    DMA2_Stream0->PAR = (uint32_t)&(SPI1->DR);            // Peripheral address
    DMA2_Stream0->M0AR = (uint32_t)&dma_staging_buffer;   // Memory address
    DMA2_Stream0->NDTR = sizeof(link_packet_t);         // 263 bytes
    
    // Enable transfer complete interrupt
    DMA2_Stream0->CR |= DMA_SxCR_TCIE;



    // 5. Enable SPI2 DMA RX request
    //SPI2->CR2 |= SPI_CR2_RXDMAEN;
    // 5. Enable SPI1 DMA RX request
    SPI1->CR2 |= SPI_CR2_RXDMAEN;
    
    // 6. Enable DMA stream
    //DMA1_Stream3->CR |= DMA_SxCR_EN;
    // 6. Enable DMA stream
    DMA2_Stream0->CR |= DMA_SxCR_EN;
    
    // 7. NOW enable SPI2
    // printf("Before SPE: SPI2->CR1 = 0x%04X\r\n", SPI2->CR1);
    // printf("SPI_CR1_SPE = 0x%04X\r\n", SPI_CR1_SPE);
    // SPI2->CR1 |= SPI_CR1_SPE;
    // printf("After SPE: SPI2->CR1 = 0x%04X\r\n", SPI2->CR1);
    
    // 7. NOW enable SPI1
    //printf("Before SPE: SPI1->CR1 = 0x%04X\r\n", SPI1->CR1);
    //printf("SPI_CR1_SPE = 0x%04X\r\n", SPI_CR1_SPE);
    SPI1->CR1 |= SPI_CR1_SPE;
    //printf("After SPE: SPI1->CR1 = 0x%04X\r\n", SPI1->CR1);
    
    // 8. Enable DMA1_Stream3 interrupt in NVIC
    // NVIC_EnableIRQ(DMA1_Stream3_IRQn);
    // printf("SPI2 + DMA initialized\r\n");
    // printf("SPI2->CR1 = 0x%04X\r\n", SPI2->CR1);
    // printf("DMA1_Stream3->CR = 0x%08lX\r\n", DMA1_Stream3->CR);
    // printf("DMA1_Stream3->NDTR = %lu\r\n", DMA1_Stream3->NDTR);
    // 8. Enable DMA2_Stream0 interrupt in NVIC
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    //printf("SPI1 + DMA initialized\r\n");
    //printf("SPI1->CR1 = 0x%04X\r\n", SPI1->CR1);
    //printf("DMA2_Stream0->CR = 0x%08lX\r\n", DMA2_Stream0->CR);
    //printf("DMA2_Stream0->NDTR = %lu\r\n", DMA2_Stream0->NDTR);
}

// DMA1 Stream 3 interrupt handler
void DMA1_Stream3_IRQHandler(void) {

    //printf("DMA ISR triggered!\r\n");  

    // Check transfer complete flag
    if(DMA1->LISR & DMA_LISR_TCIF3) {
        DMA1->LIFCR = DMA_LIFCR_CTCIF3; // Clear flag
        
        // Copy from staging buffer to ring buffer
        ring_buffer_write_packet(&global_buffer, &dma_staging_buffer);
        
        //printf("DMA received packet: src=%u, dest=%u, len=%u\r\n",
        //       dma_staging_buffer.src_id, 
        //       dma_staging_buffer.dest_id, 
        //       dma_staging_buffer.length);
        
        // Re-enable DMA for next packet
        DMA1_Stream3->CR &= ~DMA_SxCR_EN;
        while(DMA1_Stream3->CR & DMA_SxCR_EN);
        DMA1_Stream3->NDTR = sizeof(link_packet_t);
        DMA1_Stream3->CR |= DMA_SxCR_EN;
    }
}

// DMA2 Stream 0 interrupt handler
void DMA2_Stream0_IRQHandler(void) {
    //printf("DMA ISR triggered!\r\n");
    
    // Check transfer complete flag
    if(DMA2->LISR & DMA_LISR_TCIF0) {
        DMA2->LIFCR = DMA_LIFCR_CTCIF0; // Clear flag
        
        // Copy from staging buffer to ring buffer
        ring_buffer_write_packet(&global_buffer, &dma_staging_buffer);
        
        //printf("DMA received packet: src=%u, dest=%u, len=%u\r\n",
        //       dma_staging_buffer.src_id, 
        //       dma_staging_buffer.dest_id, 
        //       dma_staging_buffer.length);
        
        // Re-enable DMA for next packet
        DMA2_Stream0->CR &= ~DMA_SxCR_EN;
        while(DMA2_Stream0->CR & DMA_SxCR_EN);
        DMA2_Stream0->NDTR = sizeof(link_packet_t);
        DMA2_Stream0->CR |= DMA_SxCR_EN;
    }
}