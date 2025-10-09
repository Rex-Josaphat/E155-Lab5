// Josaphat Ngoga
// jngoga@g.hmc.edu
// 10/5/2025

// Header for timer functions

#ifndef STM32L432KC_TIM_H
#define STM32L432KC_TIM_H

#include <stdint.h> 
#include <stdio.h>

#include "stm32l432xx.h"
#include "STM32L432KC_GPIO.h"


///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////
void counterON(TIM_TypeDef* TIM);
void setDelay(TIM_TypeDef* TIM, uint32_t ms);
void resetTIM(TIM_TypeDef* TIM);
void setPrescaler(TIM_TypeDef* TIM, uint32_t val);
void setArr(TIM_TypeDef* TIM, uint32_t val);

#endif
