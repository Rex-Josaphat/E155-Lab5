// Josaphat Ngoga
// jngoga@g.hmc.edu
// 10/5/2025

// Code to read quadrature encoder using and output speed with direction

#include <stdint.h>
#include <stdio.h>
#include "lib/STM32L432KC.h"

// Define Pins and Timers Used
#define signalA PA8
#define signalB PA9

// Define Parameters
#define SAMPLE_PERIOD (0.083) // Sample period in s
#define PULSES_PER_REV (408*4) // 408 for the motor * 4 for quadrature encoding

// Global Variables
volatile int currPosition = 0;
volatile int direction = 0; // 1: forward, -1: backward, 0: stopped
volatile int newPosition = 0; 
volatile int sample = 0; // Flag to start sampling
volatile int dist  = 0;
volatile double delta = 0.0; // Change in position
volatile double speedRPS = 0.0; // Speed in Rev/s

int main(void) {
    // Enable and Configure GPIO
    gpioEnable(GPIO_PORT_A);
    pinMode(signalA, GPIO_ALT); 
    pinMode(signalB, GPIO_ALT); 
    GPIOA->AFR[1] &= ~((0xF << 0) | (0xF << 4)); // clear PA8 and PA9 AF
    GPIOA->AFR[1] |=  ((1 << 0) | (1 << 4)); // Configure AF1: PA8:TIM1_CH1 and PA9:TIM1_CH2

    // Enanble and Configure TIM6 for Sampling
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN; // Enable TIM6
    setPrescaler(TIM6, 3999); // Divide input clock frequency to counter frequency of 1kHz
    setArr(TIM6, (SAMPLE_PERIOD * 1000) - 1);
    TIM6->DIER |= (1 << 0); 
    counterON(TIM6); // Start TIM6

    // Enable and Configure TIM1 for Encoder Decoding
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; // Enable TIM1
    TIM1->SMCR &= ~(0b111 << 0); // Clear SMCR bits
    TIM1->SMCR |= (0b11 << 0); // Set TIM1 to Encoder mode 3
    TIM1->CCMR1 &= ~((0b111 << 0) | (0b111 << 8)); // Clear CC1S and CC2S bits
    TIM1->CCMR1 |= ((0b01 << 0) | (0b01 << 8)); // Map TI1 to IC1; Map TI2 to IC2
    TIM1->CCER |= (1 << 0) | (1 << 4); // CC1E, CC2E
    setArr(TIM1, PULSES_PER_REV - 1);
    counterON(TIM1); // Start TIM1

    currPosition = TIM1->CNT; // Initialize current position
    
    // Interupt Enable
    __enable_irq();
    NVIC->ISER[1] |= (1 << (TIM6_DAC_IRQn - 32)); // TIM6 interrupt

    while (1) {
        if(sample) { 
            delta = (double)dist;
            // Determine direction
            if (delta > 0) {
                direction = 1; 
            } else if (delta < 0) {
                direction = -1;
            } else {
                direction = 0;
            }

            // Calculate speed in Rev/s
            speedRPS = (delta / (PULSES_PER_REV * SAMPLE_PERIOD));

            // Display speed and direction
            if (direction == 1) {
                printf("Speed: %.2f Rev/s, Direction: Forward\n", speedRPS);
            } else if (direction == -1) {
                printf("Speed: %.2f Rev/s, Direction: Backward\n", speedRPS);
            } else {
                printf("Speed: 0.00 Rev/s, Motor Stopped\n");
            }
            sample = 0; // Reset sample flag
        }
    }
    return 0;
}

// TIM6 Interrupt Handler
void TIM6_DAC_IRQHandler(void) {
    if (TIM6->SR & 1) { // UIF
        TIM6->SR &= ~(1 << 0); // Clear UIF

        newPosition = TIM1->CNT; // Read current position from TIM1
        dist = newPosition - currPosition;

        // Handle wrap-around
        if (dist > (PULSES_PER_REV / 2)) {
            dist -= PULSES_PER_REV;
        } else if (dist < -(PULSES_PER_REV / 2)) {
            dist += PULSES_PER_REV;
        } else if (dist == (PULSES_PER_REV / 2)){
            dist = (TIM1->CR1 & TIM_CR1_DIR) ? -(PULSES_PER_REV / 2)
                                                :(PULSES_PER_REV / 2);
        }
        
        currPosition = newPosition; // Update position
        sample = 1; // Set sample flag
    }
}