#include "uart.h"
#include "stm32f446xx.h"

#define UART_BAUDRATE 115200
//#define APB1_CLK      45000000  // 45 MHz (default after reset with HSI)
#define APB1_CLK      16000000  // 16 MHz HSI (actual default after reset)

void uart2_init(void) {
    // Enable GPIOA and USART2 clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    
    // Configure PA2 (TX) and PA3 (RX) as alternate function
    // PA2: AF7 (USART2_TX), PA3: AF7 (USART2_RX)
    GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
    GPIOA->MODER |= (2 << GPIO_MODER_MODER2_Pos) | (2 << GPIO_MODER_MODER3_Pos);
    
    // Set alternate function to AF7
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);
    GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL2_Pos) | (7 << GPIO_AFRL_AFSEL3_Pos);
    
    // Configure USART2
    // Disable USART2 before configuration
    USART2->CR1 &= ~USART_CR1_UE;
    
    // Set baud rate
    // USARTDIV = APB1_CLK / (16 * BAUDRATE)
    uint32_t usartdiv = APB1_CLK / UART_BAUDRATE;
    USART2->BRR = usartdiv;
    
    // Enable transmitter and receiver
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;
    
    // Enable USART2
    USART2->CR1 |= USART_CR1_UE;
}

void uart2_write(uint8_t ch) {
    // Wait until transmit data register is empty
    while (!(USART2->SR & USART_SR_TXE));
    
    // Write data to transmit register
    USART2->DR = ch;
}

void uart2_write_string(const char *str) {
    while (*str) {
        uart2_write(*str++);
    }
}
