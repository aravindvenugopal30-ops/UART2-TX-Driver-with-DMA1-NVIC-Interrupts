#include <stdint.h>
#include <stdio.h>
#include "led1.h"

#define ENABLE_GLOBAL_INTERRUPT()  __asm volatile ("CPSIE i" : : : "memory")
#define DISABLE_GLOBAL_INTERRUPT() __asm volatile ("CPSID i" : : : "memory")

#define PERIPH_BASE                 (0x40000000UL)

// RCC
#define RCC                         (0x40023800UL)

#define AHB1_OFFSET                 (0x20000UL)
#define AHB1_BASE                   (PERIPH_BASE + AHB1_OFFSET)

#define GPIOA_OFFSET                (0x0000UL)
#define GPIOA_BASE                  (AHB1_BASE + GPIOA_OFFSET)

#define AHB1ENR_OFFSET              (0x30UL)
#define RCC_AHB1                    (*(volatile unsigned int *)(AHB1ENR_OFFSET + RCC))

#define MODER_OFFSET                (0x00)
#define GPIOA_MODER                 (*(volatile unsigned int *)(MODER_OFFSET + GPIOA_BASE))

#define APB1_OFFSET                 (0x0000UL)
#define APB1_BASE                   (PERIPH_BASE + APB1_OFFSET)

#define USART2_OFFSET               (0x4400UL)
#define USART2_BASE                 (APB1_BASE + USART2_OFFSET)

#define APB1ENR_OFFSET              (0x40UL)
#define RCC_APB1ENR                 (*(volatile unsigned int *)(RCC + APB1ENR_OFFSET))

#define AFRL_OFFSET                 (0x20UL)
#define GPIOA_AFRL                  (*(volatile unsigned int *)(GPIOA_BASE + AFRL_OFFSET))

#define BRR_OFFSET                  (0x08UL)
#define USART2_BRR                  (*(volatile unsigned int *)(USART2_BASE + BRR_OFFSET))

#define CR1_OFFSET                  (0x0CUL)
#define USART2_CR1                  (*(volatile unsigned int *)(USART2_BASE + CR1_OFFSET))

#define DR_OFFSET                   (0x04UL)
#define USART2_DR                   (*(volatile unsigned int *)(DR_OFFSET + USART2_BASE))

#define SR_OFFSET                   (0x00UL)
#define USART2_SR                   (*(volatile unsigned int *)(USART2_BASE + SR_OFFSET))


#define UARTENR                     (1U<<17)
#define GPIOAENR                    (1U<<0)

#define SYS_FR                      16000000
#define APB1_CLK                    SYS_FR
#define UART_BAUDRATE               115200

#define CR1_TE                      (1U<<3)
#define UASRT_EN                    (1U<<13)
#define SR_TXE                      (1<<7)
#define SR_RXNE                     (1<<5)


//DMA

#define DMA1                        (0x40026000UL)
#define DMA1_S6CR                   (*(volatile uint32_t *)(DMA1 + 0xA0UL))

#define HIFCR_OFFSET                (0x00CUL)
#define DMA1_HIFCR                  (*(volatile uint32_t *)(DMA1 + HIFCR_OFFSET))

#define PAR_OFFSET                  (0xA8UL)
#define DMA1_PAR                    (*(volatile uint32_t *)(DMA1 + PAR_OFFSET))

#define M0AR_OFFSET                 (0xACUL)
#define DMA1_M0AR                   (*(volatile uint32_t *)(DMA1 + M0AR_OFFSET))

#define NDTR_OFFSET                 (0x0A4UL)
#define DMA1_NDTR                   (*(volatile uint32_t *)(DMA1 + NDTR_OFFSET))

#define FCR_OFFSET                  (0x0B4UL)
#define DMA_FCR                     (*(volatile uint32_t *)(DMA1 + FCR_OFFSET))

#define CR3_OFFSET                  (0x14UL)
#define USART2_CR3                  (*(volatile uint32_t *)(USART2_BASE +CR3_OFFSET))

#define NVIC_ISER0                (*(volatile uint32_t *)(0xE000E104UL))

#define HISR_OFFSET                (0x004UL)
#define DMA_HISR                   (*(volatile uint32_t *)(DMA1 + HISR_OFFSET))

#define HIFCR_OFFSET               (0x00CUL)
#define DMA_HIFCR                  (*(volatile uint32_t *)(DMA1 + HIFCR_OFFSET))

static void uart_set_baudrate(uint32_t periphclk, uint32_t BaudRate);
static uint16_t compute_uart_div(uint32_t periphclk, uint32_t BaudRate);
void uar2_tx_init(void);
void uart2_write(int ch);
void uart2_write_string(char *str);
char uart2_read(void);

void uart_rx_init();
void dma1_stream6_init(uint32_t src,uint32_t dst, uint32_t len);

volatile uint8_t flag_C =0;

int main(){

	ENABLE_GLOBAL_INTERRUPT();
	led_init();
	led_1bl();
	//for(volatile int i=0;i<=5000;i++){}


    uar2_tx_init();

	char masg[]="\n__SUIIIIIIiii__";

    dma1_stream6_init((uint32_t) masg,(uint32_t)&USART2_DR,sizeof(masg)-1);

    /*if(flag_C==1){
        		led_1bl();

        		flag_C=0;*/
    while (1)
    {
    	if(DMA_HISR & (1U<<21)){
    		DMA_HIFCR=(1U<<21);
    		while(!(USART2_SR &(1U<<6))){}
    		USART2_CR3 &=~(1U<<7);
		led_1bl();
		uart2_write_string("\nCompleted");

    	}
}
}


void uar2_tx_init(void){
    RCC_AHB1 |= GPIOAENR;

    GPIOA_MODER &= ~(1U << 4);
    GPIOA_MODER |= (1U << 5);

    GPIOA_AFRL |= (1U << 8);
    GPIOA_AFRL |= (1U << 9);
    GPIOA_AFRL |= (1U << 10);
    GPIOA_AFRL &= ~(1U << 11);

    RCC_APB1ENR |= UARTENR;

    uart_set_baudrate(APB1_CLK, UART_BAUDRATE);

    USART2_CR1 = CR1_TE;
    USART2_CR1 |= UASRT_EN;
}


void uart2_write(int ch){
    while(!(USART2_SR & SR_TXE)){}
    USART2_DR = (ch&0xFF);
}


void uart2_write_string(char *str) {
    while (*str) {
        uart2_write(*str);
        str++;
    }
}

static void uart_set_baudrate(uint32_t periphclk, uint32_t BaudRate)
{
    USART2_BRR = compute_uart_div(periphclk, BaudRate);
}

static uint16_t compute_uart_div(uint32_t periphclk, uint32_t BaudRate)
{
    return ((periphclk + (BaudRate / 2U)) / BaudRate);
}

void uart_rx_init(){

	 RCC_AHB1 |= GPIOAENR;

	 GPIOA_MODER &=~(1U<<5);
	 GPIOA_MODER |=(1U<<6);

	 GPIOA_AFRL |=(1U<<12);
	 GPIOA_AFRL |=(1U<<13);
	 GPIOA_AFRL |=(1U<<14);
	 GPIOA_AFRL &=~(1U<<15);

	  RCC_APB1ENR |= UARTENR;


	    uart_set_baudrate(APB1_CLK, UART_BAUDRATE);

	    USART2_CR1 |=(1U<<2);
	    USART2_CR1 |=(1U<<13);


}

char uart2_read(void)
{

    while (!(USART2_SR & SR_RXNE));
    return (char)(USART2_DR & 0xFF);
}

void dma1_stream6_init(uint32_t src,uint32_t dst, uint32_t len){

	//enable clk access to DMA
	RCC_AHB1|=(1U<<21);
	//disable DMA1 stream6
	DMA1_S6CR &=~(1U<<0);
	//clear all interrupt flag for of stream6
	DMA1_HIFCR |=(1U<<16);
	DMA1_HIFCR |=(0x0F<<18);
	//set destination buzzer
	DMA1_PAR=dst;
	//set source buffer
	DMA1_M0AR=src;
	//set length
	DMA1_NDTR=len;
	//select stream6 CH4
	DMA1_S6CR|=(0x4U<<25);
	//enable memory increment
	DMA1_S6CR|=(0x1U<<10);
	//Configure transfer direction
	DMA1_S6CR |= (1U << 6);
	DMA1_S6CR &= ~(1U << 7);
	//enable DMA transfer complete interrupt
	DMA1_S6CR|=(1U<<4);
	//Enable direct mode and disable FIFO
	DMA_FCR &=~(1U<<2);
	//enable DMA1 stream6
	DMA1_S6CR |=(1U<<0);
	//enable UART2 transmitter DMA
	USART2_CR3 |=(1U<<7);
	//DMA interrupt enabled in NVIC
	NVIC_ISER0 |=(1U<<15);
}

void DMA1_Stream6_IRQHandler(void){
	// check for transfer complete interrupt
	if(DMA_HISR &(1U<<21)){
		//clear flag
		//flag_C=1;

	}


}



