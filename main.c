/**
  @file    main.c
  @author  h0rr0rr_drag0n
  @version V0.0.1
  @date    22-Aug-2011
  @brief   Template of Linux project for STM32VLDiscovery
**/

#include "stm32f10x.h"
#include "usart.h"



volatile uint32_t TimingDelay;

void Delay(uint32_t delay){
  TimingDelay = delay;
  while(TimingDelay !=0);
}

void SysTick_Handler (void){
  if ( TimingDelay != 0x00)
    TimingDelay --;
}



void usart_puts(USART_TypeDef *USARTx, char *str){
  while(*str){
    usart_putc(USARTx,*str);
    str++;
  }
}




int main(void) {


  if(SysTick_Config(SystemCoreClock/1000)) while(1); // Initialize system timer

  usart_open(USART1,9600);
  


  /* Set pins 8 and 9 at PORTC to high level */
  
  while (1) {
    usart_puts(USART1, "Hello, world!\n\r");
    Delay(250);
  }
}

