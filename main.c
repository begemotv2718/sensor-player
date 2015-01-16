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
#include <string.h>
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
#define INPUT_BUFFER_LEN 50

int16_t dav0[NSAMPLES-1];
int16_t dav1[NSAMPLES-1];

void do_measurement_cycle(int16_t *dav0, int16_t *dav1){
    uint16_t ain[NSAMPLES];
    int i;
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
}

void normal_operate(int32_t threshold ){
    do_measurement_cycle(dav0,dav1);
    //uint16_t dain[NSAMPLES-1];
    if(dav0[1]*100/NCYCLES<threshold){
      xprintf("Pressed!\n");
      GPIO_WriteBit(GPIOC,GPIO_Pin_9,1);
    }else{
      GPIO_WriteBit(GPIOC,GPIO_Pin_9,0);
    }
}

#define MEASUREMENT_CYCLES 100
void report_measurement(){
  int i,j;
  int32_t sum;
  int32_t sumsq;
  int32_t min;
  int32_t max;
  sum=0;
  sumsq=0;
  min = 1000000;
  max =0;
  for(j=1;j<MEASUREMENT_CYCLES;j++){
    do_measurement_cycle(dav0,dav1);
    sum+=dav0[1];
    sumsq+=dav0[1]*dav0[1];
    if(min>dav0[1]){
      min=dav0[1];
    }
    if(max<dav0[1]){
      max=dav0[1];
    }
    xprintf("Switching to 0 Differences: ");
    int i;
    for(i=0; i<NSAMPLES-1; i++){
      xprintf("%d ",dav0[i]*100/NCYCLES);
    }
    xprintf("\n");
    xprintf("Switching to 1 Differences: ");
    for(i=0; i<NSAMPLES-1; i++){
      xprintf("%d ",dav1[i]*100/NCYCLES);
    }
    xprintf("\n");
    xprintf("j=%d sum = %d sumsq=%d\n",j,sum,sumsq);
  }
  xprintf("Average %d min %d max %d \n",sum/MEASUREMENT_CYCLES,min,max);

}  


enum state_t {ST_NORMAL, ST_MEASURE, ST_SET, ST_COMMAND_LOOP} state;

int main(void) {
  xfunc_in = usart_getc;
  xfunc_out = usart_putc;
  GPIO_port = GPIOC;
  GPIO_control_pin = GPIO_Pin_5; 
  state =ST_NORMAL;

  if(SysTick_Config(SystemCoreClock/100000)) while(1); // Initialize system timer
  init_adc();
  init_GPIOC_pin();

  usart_open(USART1,9600);
  


  uint8_t ch;
  int32_t threshold = 23000; 
  char buffer[INPUT_BUFFER_LEN];
  char *conv_pointer;
  xprintf("Starting conversion\n");
  GPIO_WriteBit(GPIO_port,GPIO_control_pin,0);
  uint16_t j=0;
  while (1) {
    switch(state){
      case ST_NORMAL:
        normal_operate(threshold);
        if(usart_poll_getc(&ch)){
          if(ch == 13 || ch == 10){
            state = ST_COMMAND_LOOP;
          }
          xprintf("Read character %d",(int32_t)ch);
        }
        /*
        j++;
        if(j>100){
          j=0;
          xprintf("Another cycle passed eventless\n");
        }
        */
        break;
      case ST_COMMAND_LOOP:
        xprintf("Command>");
        xgets(buffer,INPUT_BUFFER_LEN);
        conv_pointer = buffer;
        if(strncmp(buffer,"measure",7)==0){
          state=ST_MEASURE;
        }else if(strncmp(buffer,"normal",6)==0){
          state=ST_NORMAL;
        }else if(strncmp(buffer,"set",3)==0){
          conv_pointer+=3;
          if(!xatoi(&conv_pointer,&threshold)){
            threshold=23000;
          }
        }
        break;
      case ST_MEASURE:
        report_measurement();
        state=ST_COMMAND_LOOP;
        break;
      default:
        break;

   }

  }
}

