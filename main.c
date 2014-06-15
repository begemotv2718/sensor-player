/**
  @file    main.c
  @author  h0rr0rr_drag0n
  @version V0.0.1
  @date    22-Aug-2011
  @brief   Template of Linux project for STM32VLDiscovery
**/

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"



volatile uint32_t TimingDelay;

void Delay(uint32_t delay){
  TimingDelay = delay;
  while(TimingDelay !=0);
}

void SysTick_Handler (void){
  if ( TimingDelay != 0x00)
    TimingDelay --;
}


void usart_init() {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitTypeDef GPIOA_init_params;

  GPIO_StructInit(&GPIOA_init_params);

  GPIOA_init_params.GPIO_Speed = GPIO_Speed_2MHz;

  GPIOA_init_params.GPIO_Mode = GPIO_Mode_AF_PP;

  GPIOA_init_params.GPIO_Pin = GPIO_Pin_9;

  GPIO_Init(GPIOA,&GPIOA_init_params);

  GPIOA_init_params.GPIO_Mode = GPIO_Mode_IN_FLOATING;

  GPIOA_init_params.GPIO_Pin = GPIO_Pin_10;

  GPIO_Init(GPIOA, &GPIOA_init_params);


  USART_InitTypeDef USART_init_params;

  USART_StructInit(&USART_init_params);

  USART_init_params.USART_BaudRate = 9600;

  USART_init_params.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1,&USART_init_params);

  USART_Cmd(USART1, ENABLE);
}

void usart_putc(int c){
    while ( USART_GetFlagStatus (USART1 , USART_FLAG_TXE ) == RESET);
    USART1->DR = (c & 0xff);
}

int usart_getc(void){
  while ( USART_GetFlagStatus (USART1 , USART_FLAG_RXNE ) == RESET);
  return USART1->DR && 0xff;
}


void usart_puts(char *str){
  while(*str){
    usart_putc(*str);
    str++;
  }
}




int main(void) {


  if(SysTick_Config(SystemCoreClock/1000)) while(1); // Initialize system timer

  usart_init();
  


  /* Set pins 8 and 9 at PORTC to high level */
  
  while (1) {
    usart_puts("Hello, world!\n\r");
    Delay(250);
  }
}

