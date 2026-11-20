/*
 * led.c
 *
 *  Created on: Nov 16, 2026
 *      Author: ARAVIND
 */
#include<stdio.h>
#include<stdint.h>

#include"led1.h"

#define PERIPH_BASE        (0x40000000UL)
#define AHB1PERIPH_OFFSET  (0x00020000UL)
#define AHB1PERIPH_BASE    (PERIPH_BASE + AHB1PERIPH_OFFSET)
#define GPIOA_OFFSET       (0x0000)

#define GPIOA              (AHB1PERIPH_BASE + GPIOA_OFFSET)

#define RCC_OFFSET          (0x3800UL)
#define RCC                 (AHB1PERIPH_BASE+ RCC_OFFSET )

#define AHB1ENR_OFFSET      (0x30UL)
#define RCC_AHB1          (*(volatile unsigned int *)(RCC + AHB1ENR_OFFSET))

#define MODE_R_OFFSET       (0x00UL)
#define GPIOA_MODER       (*(volatile unsigned int *)(GPIOA + MODE_R_OFFSET))

#define OD_R_OFFSET         (0x14UL)
#define GPIOA_OD_R          (*(volatile unsigned int *) (GPIOA + OD_R_OFFSET))

#define GPIOAEN             (1U<<0)

#define PIN5                (1U<<5)
#define LED_PIN              PIN5


void led_init(){

	RCC_AHB1 |=(1U<<0);

		GPIOA_MODER |=(1U<<10);
		GPIOA_MODER &=~(1U<<11);

}
void led_1bl(){

	GPIOA_OD_R |=LED_PIN;
			for(int i=0;i<1000000;i++){}


			GPIOA_OD_R &=~LED_PIN;

}






