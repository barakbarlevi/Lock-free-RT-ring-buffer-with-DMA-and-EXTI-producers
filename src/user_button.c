#include "user_button.h"
#include "stm32f446xx.h"

void user_button_init(void) {
    // Enable clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;   // GPIOC clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;  // Enable System Configuration Controller Clock (SYSCFG)
    
    // Configure PC13 as input (default state, but explicit). Set PC13 as Input (00 in MODER)
    GPIOC->MODER &= ~(3 << (13 * 2)); // Clear bits 26-27
    
    // Connect EXTI13 to PC13
    SYSCFG->EXTICR[3] &= ~(0xF << 4); // Clear bits
    SYSCFG->EXTICR[3] |= (2 << 4);  // EXTICR[3] controls EXTI12-15, PC=2
    
    // Configure EXTI13
    EXTI->IMR |= (1 << 13);    // Unmask interrupt
    EXTI->RTSR |= (1 << 13);   // Rising edge trigger
    EXTI->FTSR &= ~(1 << 13);  // Disable falling edge
    // EXTI->FTSR |= (1 << 13);   // Falling edge (button press pulls to GND)
    
    // Enable in NVIC
    NVIC_EnableIRQ(EXTI15_10_IRQn);  // IRQ number 40
    NVIC_SetPriority(EXTI15_10_IRQn, 0); // High priority
}