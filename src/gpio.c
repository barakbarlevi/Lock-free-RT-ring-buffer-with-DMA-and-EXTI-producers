#include "gpio.h"
#include "stm32f446xx.h"

void gpio_init_led(void) {
    // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Configure PA5 as output
    GPIOA->MODER &= ~(3 << (LED_PIN * 2)); // Clear mode bits. thr *2 is because each gpio pin uses 2 bits in the MODER register. so bits 11-10 decide the mode of pin 5 (led)
    // mask = (3 << (LED_PIN * 2))
    // python: bin(mask) 
    // '0b110000000000'
    // inverted_mask = ~mask & 0xFFFFFFFF  # the & 0xFFFFFFFF is to use 32-bit
    // So what the line above does is
    // MODER     = 0bxxxxxxxxxxxxxxxxxxxxxxxx11xxxxxxxxxx  (bits 10-11 might be set)
    // ~mask     = 0b11111111111111111111001111111111     (bits 10-11 are 0)
    // Result    = 0bxxxxxxxxxxxxxxxxxxxxxxxx00xxxxxxxxxx  (bits 10-11 now cleared)
    // Clear bits 10 and 11 of MODER (the mode bits for PA5), leaving all other pins' modes unchanged
    // Must clear first. the bits might have been set to 11, who knows

    /* 00 = Input mode
    01 = Output mode
    10 = Alternate function mode
    11 = Analog mode */
    GPIOA->MODER |= (1 << (LED_PIN * 2)); // set only bit 10. set to output mode (01)

    // XXXX
    GPIOA->OTYPER &= ~(1 << LED_PIN);

    // XXXX
    GPIOA->OSPEEDR &= ~(3 << LED_PIN * 2);

    // No pull-up/pull-down
    GPIOA->PUPDR &= ~(3 << LED_PIN *2);
}

void gpio_toggle_led(void) {
    GPIOA->ODR ^= (1 << LED_PIN); // XOR to toggle
    // ODR - Output Data Register
    // Each bit controls the output state of a GPIO pin
    // bit 0 = PA0, ..., bit 5 = PA5
    // 1 is high, 0 is low
    // XORing with 0 makes the bit remain as is. XORing with 1 flips the bit

    /* python:
    mask = mask = 1 << 5
    >>> bin(mask)
    '0b100000' # only bit 5 is set

    step 2: xor with ODR
    case 1: LED is currently off (bit 5 = 0):
    ODR   = 0bxxxxxxxxxxxxxxxxxxxxxxxx0xxxxx
    mask  = 0b00000000000000000000000100000
    XOR   = 0bxxxxxxxxxxxxxxxxxxxxxxxx1xxxxx
    case 2: the opposite. in both cases, bit 5 flips


    */ 

}