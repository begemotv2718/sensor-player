/**
  @file    main.c
  @author  h0rr0rr_drag0n
  @version V0.0.1
  @date    22-Aug-2011
  @brief   Template of Linux project for STM32VLDiscovery
**/

#include "stm32f10x.h"
#include "usart.h"
#include "spi.h"


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

void spiInitSS(){
  GPIO_InitTypeDef GPIO_init;
  GPIO_StructInit(&GPIO_init);

  GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_init.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_init.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOA, &GPIO_init);
}

uint8_t txbuf[10], rxbuf[10];

int main(void) {
  const uint8_t *str = "Hello\n";

  if(SysTick_Config(SystemCoreClock/1000)) while(1); // Initialize system timer

  usart_open(USART1,9600);
  
  spiInitSS();
  spiInit(SPI1);


  /* Set pins 8 and 9 at PORTC to high level */
  int i;
  while (1) {
    for(i=0;i<sizeof(str);i++){
      txbuf[i]=str[i];
      rxbuf[i]=0;
    }
    GPIO_WriteBit(GPIOA,GPIO_Pin_4,0);
    spiReadWrite(SPI1,rxbuf,txbuf,7,SPI_SLOW);
    GPIO_WriteBit(GPIOA,GPIO_Pin_4,1);
    usart_puts(USART1, rxbuf);
    Delay(250);
  }
}

