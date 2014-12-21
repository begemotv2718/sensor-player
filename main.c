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

  /*LED port*/
  GPIO_init_params.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_init_params.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_init_params.GPIO_Pin = GPIO_Pin_9;
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

#define NSAMPLES 10
#define NCYCLES 5

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
    uint16_t ain[NSAMPLES];
    //uint16_t dain[NSAMPLES-1];
    int16_t dav0[NSAMPLES-1];
    int16_t dav1[NSAMPLES-1];
    for(i=0;i<NSAMPLES-1;i++){
      dav0[i]=0;
    }
    for(i=0;i<NSAMPLES-1;i++){
      dav1[i]=0;
    }
    int cycle;
    for(cycle=0;cycle<NCYCLES;cycle++){
      start_conversion(NSAMPLES,ain,switch_to_0);
      while(!conv_finished());
      for(i=0;i<NSAMPLES-1;i++){
        dav0[i]+=(int16_t)ain[i]-(int16_t)ain[i+1];
      }
      Delay(50);
      start_conversion(NSAMPLES,ain,switch_to_1);
      while(!conv_finished());
      for(i=0;i<NSAMPLES-1;i++){
        dav1[i]+=(int16_t)ain[i+1]-(int16_t)ain[i];
      }
    }
    if(dav0[1]*100/NCYCLES<23000){
      xprintf("Pressed!\n");
      GPIO_WriteBit(GPIOC,GPIO_Pin_9,1);
      xprintf("Switching to 0 Differences: ");
      for(i=0; i<NSAMPLES-1; i++){
        xprintf("%d ",dav0[i]*100/NCYCLES);
      }
      xprintf("\n");
      xprintf("Switching to 1 Differences: ");
      for(i=0; i<NSAMPLES-1; i++){
        xprintf("%d ",dav1[i]*100/NCYCLES);
      }
      xprintf("\n");
    }else{
      GPIO_WriteBit(GPIOC,GPIO_Pin_9,0);
    }
    /*
    xprintf("Switching to 0 Differences: ");
    for(i=0; i<NSAMPLES-1; i++){
      xprintf("%d ",dav0[i]*100/NCYCLES);
    }
    xprintf("\n");
    xprintf("Switching to 1 Differences: ");
    for(i=0; i<NSAMPLES-1; i++){
      xprintf("%d ",dav1[i]*100/NCYCLES);
    }
    xprintf("\n");
    */
  }
}

