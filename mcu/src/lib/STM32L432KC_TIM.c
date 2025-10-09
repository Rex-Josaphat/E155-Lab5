// Josaphat Ngoga
// jngoga@g.hmc.edu
// 10/5/2025

// Source code for timer functions

#include <stdint.h>
#include <stdio.h>
#include "STM32L432KC_TIM.h"

// Clear TIM count status
void resetTIM(TIM_TypeDef* TIM) {
    TIM->SR &= ~(1 << 0); // Clear TIM UIF
    TIM->CNT = 0; // Reset TIM counter to 0
}

// Divide input SYS Clock (4MHZ) to required TIM Frequency
void setPrescaler(TIM_TypeDef* TIM, uint32_t val) {
    TIM->PSC = val;
    TIM->EGR |= (1 << 0); // update event (UG bit)    
}   

// Enable counter for Specified TIM
void counterON(TIM_TypeDef* TIM) {
    TIM->CR1 |= (1 << 0);
}

// Set counter
void setArr(TIM_TypeDef* TIM, uint32_t val) {
    TIM->ARR = val;
}

// Implement ms delay counter
void setDelay(TIM_TypeDef* TIM, uint32_t ms) {
    setArr(TIM, ms-1);
    resetTIM(TIM);
    counterON(TIM); // Enable Counter for specified TIM
    while (!(TIM->SR & 1)) {} // Wait for counter overflow
}