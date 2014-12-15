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
  while (1) {
    GPIO_WriteBit(GPIO_port,GPIO_control_pin,0);
    uint16_t ain[10];
    for(i=0; i<10;i++){
      ain[i] = ADC_GetConversionValue(ADC1);
      Delay(2);
    }
    xprintf("Switching to 0 adc value: ");
    for(i=0;i<10;i++){
      xprintf("%d ",ain[i]);
    }
    xprintf("\n");
    Delay(800000);
    GPIO_WriteBit(GPIO_port,GPIO_control_pin,1);
    for(i=0; i<10;i++){
      ain[i] = ADC_GetConversionValue(ADC1);
      Delay(2);
    }
    xprintf("Switching to 1 adc value: ");
    for(i=0;i<10;i++){
      xprintf("%d ",ain[i]);
    }
    xprintf("\n");
    Delay(800000);
  }
}

