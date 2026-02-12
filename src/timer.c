#include "timer.h"
#include "stm32f446xx.h"

// TIM2 runs at APB1 clock (16 MHz after reset)
// We'll configure it to trigger an interrupt every 500ms

volatile uint32_t timer_ticks = 0;

void timer2_init(void) {
    // Enable TIM2 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Configure timer for 500 ms (2 Hz)
    // APB1 clock = 16MHz
    // Prescaler: 16000 - 1 (divide by 16000) -> 1 KHz tick rate
    // Auto-reload: 500 - 1 (count to 500) -> 500[ms] period
    TIM2->PSC = (uint32_t)(PRESCALER_VALUE - 1);
    TIM2->ARR = (uint32_t)(AUTO_RELOAD_VALUE - 1);

    // XXXX
    TIM2->DIER |= TIM_DIER_UIE;

    // XXXX
    NVIC_EnableIRQ(TIM2_IRQn);

    // XXXX
    TIM2->CR1 |= TIM_CR1_CEN;
}

// TIM2 interrupt handler
void TIM2_IRQHandler(void) {
    // XXXX
    if(TIM2->SR & TIM_SR_UIF) {
        // XXXX
        TIM2->SR &= ~TIM_SR_UIF;

        // XXXX
        timer_ticks++;
    }
}
