/**
  @file    main.c
  @author  h0rr0rr_drag0n
  @version V0.0.1
  @date    22-Aug-2011
  @brief   Template of Linux project for STM32VLDiscovery
**/
#include "stm32f10x.h"
#include "usart.h"
#include "adc.h"
#include "xprintf.h"
//#include "spi.h"


volatile uint32_t TimingDelay;

void Delay(uint32_t delay){
  TimingDelay = delay;
  while(TimingDelay !=0);
}

void SysTick_Handler (void){
  if ( TimingDelay != 0x00)
    TimingDelay --;
}

static GPIO_TypeDef *GPIO_port;
static uint16_t GPIO_control_pin;

void init_GPIOC_pin(void){
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_InitTypeDef GPIO_init_params;
  GPIO_StructInit(&GPIO_init_params);


  GPIO_init_params.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_init_params.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_init_params.GPIO_Pin = GPIO_control_pin;
  GPIO_Init(GPIO_port, &GPIO_init_params);
}

void usart_puts(USART_TypeDef *USARTx, char *str){
  while(*str){
    usart_putc((uint8_t)*str);
    str++;
  }
}

void switch_to_1(void){
    GPIO_WriteBit(GPIOC,GPIO_Pin_5,1);
}

void switch_to_0(void){
    GPIO_WriteBit(GPIOC,GPIO_Pin_5,0);
}


void nop(void){
}

int main(void) {
  xfunc_in = usart_getc;
  xfunc_out = usart_putc;
  GPIO_port = GPIOC;
  GPIO_control_pin = GPIO_Pin_5; 

  if(SysTick_Config(SystemCoreClock/100000)) while(1); // Initialize system timer
  init_adc();
  init_GPIOC_pin();

  usart_open(USART1,9600);
  


  int i;
  xprintf("Starting conversion\n");
  while (1) {
    GPIO_WriteBit(GPIO_port,GPIO_control_pin,0);
    uint16_t ain[10];
    uint16_t dain[9];
    start_conversion(10,ain,switch_to_0);
    while(!conv_finished());
    /*
    xprintf("Switching to 0 adc value: ");
    for(i=0;i<10;i++){
      xprintf("%d ",ain[i]);
    }
    */
    xprintf("\n");
    for(i=0;i<9;i++){
      dain[i]=ain[i]-ain[i+1];
    }
    xprintf("Switching to 0 Differences: ");
    for(i=0; i<9; i++){
      xprintf("%d ",dain[i]);
    }
    xprintf("\n");
    Delay(500);
    /*
    start_conversion(10,ain,nop);
    while(!conv_finished());
    xprintf("Switching to 0 adc value after 1/1000 sec: ");
    for(i=0;i<10;i++){
      xprintf("%d ",ain[i]);
    }
    xprintf("\n");
    Delay(10000);
    start_conversion(10,ain,nop);
    while(!conv_finished());
    xprintf("Switching to 0 adc value after 1/10 sec: ");
    for(i=0;i<10;i++){
      xprintf("%d ",ain[i]);
    }
    xprintf("\n");
    Delay(800000);
    */
    start_conversion(10,ain,switch_to_1);
    while(!conv_finished());
    /*
    xprintf("Switching to 1 adc value: ");
    for(i=0;i<10;i++){
      xprintf("%d ",ain[i]);
    }
    xprintf("\n");
    */
    for(i=0;i<9;i++){
      dain[i]=ain[i+1]-ain[i];
    }
    xprintf("Switching to 1 Differences: ");
    for(i=0; i<9; i++){
      xprintf("%d ",dain[i]);
    }
    xprintf("\n");
    Delay(100500);
    /*
    start_conversion(10,ain,switch_to_1);
    while(!conv_finished());
    xprintf("Switching to 1 adc value after 1/1000 sec: ");
    for(i=0;i<10;i++){
      xprintf("%d ",ain[i]);
    }
    xprintf("\n");
    Delay(10000);
    start_conversion(10,ain,switch_to_1);
    while(!conv_finished());
    xprintf("Switching to 1 adc value after 2/10 sec: ");
    for(i=0;i<10;i++){
      xprintf("%d ",ain[i]);
    }
    xprintf("\n");
    Delay(800000);
    */
  }
}

