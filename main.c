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
#include <stdlib.h>
#include "sqrt.h"
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

struct adc_channel operation_channels[] = {
  {
    .pp_port = GPIOC,
    .pp_pin = GPIO_Pin_5,
    .adc_port = GPIOA,
    .adc_pin = GPIO_Pin_6,
    .channel = ADC_Channel_6 
  },
  {
    .adc_port =NULL
  }
};

void init_GPIOC_pin(void){
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_InitTypeDef GPIO_init_params;
  GPIO_StructInit(&GPIO_init_params);


  /*LED port*/
  GPIO_init_params.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_init_params.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_init_params.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOC, &GPIO_init_params);
}

void usart_puts(USART_TypeDef *USARTx, char *str){
  while(*str){
    usart_putc((uint8_t)*str);
    str++;
  }
}

int cmpint16_t(const void *a, const void *b){
  int16_t va,vb;
  va=*(int16_t*)a;
  vb=*(int16_t*)b;
  if(va==vb) return 0;
  return va>vb?1:-1;
}

int16_t median(int16_t arr[], int16_t nmemb){
  qsort(arr,nmemb,sizeof(int16_t),cmpint16_t);
  if(nmemb%2==0){
    return (arr[nmemb/2]+arr[nmemb/2+1])/2;
  }else{
    return (arr[nmemb/2]);
  }
}

void switch_to_1(struct adc_channel *ch){
    GPIO_WriteBit(ch->pp_port,ch->pp_pin,1);
}

void switch_to_0(struct adc_channel *ch){
    GPIO_WriteBit(ch->pp_port,ch->pp_pin,0);
}


void nop(void){
}

#define NSAMPLES 4 
#define NCYCLES 10
#define NCHANNELS 1
#define INPUT_BUFFER_LEN 250
#define CHANNEL_DELAY 4

int16_t dav0[NCHANNELS][NSAMPLES-1][NCYCLES];
int16_t dav1[NCHANNELS][NSAMPLES-1][NCYCLES];

void do_measurement_cycle(int16_t dav0[][NSAMPLES-1][NCYCLES], int16_t dav1[][NSAMPLES-1][NCYCLES]){
    uint16_t ain[NSAMPLES];
    int i,j,ch;
    for(ch=0;ch<NCHANNELS;ch++){
      for(i=0;i<NSAMPLES-1;i++){
        for(j=0;j<NCYCLES;j++){
          dav0[ch][i][j]=0;
        }
      }
    }
    for(ch=0;ch<NCHANNELS;ch++){
      for(i=0;i<NSAMPLES-1;i++){
        for(j=0;j<NCYCLES;j++){
          dav1[ch][i][j]=0;
        }
      }
    }
    int cycle;
    for(cycle=0;cycle<NCYCLES;cycle++){
      for(ch=0;ch<NCHANNELS;ch++){
        start_conversion(NSAMPLES,ain,&operation_channels[ch],switch_to_0);
        while(!conv_finished());
        for(i=0;i<NSAMPLES-1;i++){
          dav0[ch][i][cycle]=(int16_t)ain[i]-(int16_t)ain[i+1];
        }
      }
      Delay(60-CHANNEL_DELAY*NCHANNELS);
      for(ch=0;ch<NCHANNELS;ch++){
        start_conversion(NSAMPLES,ain,&operation_channels[ch],switch_to_1);
        while(!conv_finished());
        for(i=0;i<NSAMPLES-1;i++){
          dav1[ch][i][cycle]=(int16_t)ain[i+1]-(int16_t)ain[i];
        }
      }
      Delay(60-CHANNEL_DELAY*NCHANNELS);
    }
}

int32_t process_measure( int16_t d[]){
  return median(d,NCYCLES);
}

uint16_t normal_operate(int32_t *threshold ){
    int ch;

    do_measurement_cycle(dav0,dav1);
    for(ch=0;ch<NCHANNELS;ch++){
      if(process_measure(dav0[ch][1])<threshold[ch]){
        xprintf("Pressed!\n");
        return ch+1;
      }
    }
    return 0;
}

#define MEASUREMENT_CYCLES 400
void report_measurement(){
  int j;
  uint64_t sum[NCHANNELS];
  uint64_t sumsq[NCHANNELS];
  uint32_t min[NCHANNELS];
  uint32_t max[NCHANNELS];
  uint16_t val;
  int chn;
  for(chn=0;chn<NCHANNELS;chn++){
    sum[chn]=0;
    sumsq[chn]=0;
    min[chn] = 0xffffffff;
    max[chn] =0;
  }
  xprintf("Starting measurement\n");
  for(j=1;j<=MEASUREMENT_CYCLES;j++){
    do_measurement_cycle(dav0,dav1);
    for(chn=0;chn<NCHANNELS;chn++){
      val =median(dav0[chn][1],NCYCLES);
      sum[chn]+=val;
      sumsq[chn]+=val*val;
      if(min[chn]>val){
        min[chn]=val;
      }
      if(max[chn]<val){
        max[chn]=val;
      }
      xprintf("Switching to 0 Differences channel %d: ",chn);
      int i;
      for(i=0; i<NSAMPLES-1; i++){
        xprintf("%d ",median(dav0[chn][i],NCYCLES));
      }
      xprintf("\n");
      xprintf("Switching to 1 Differences channel %d: ",chn);
      for(i=0; i<NSAMPLES-1; i++){
        xprintf("%d ",median(dav1[chn][i],NCYCLES));
      }
      xprintf("\n");
      xprintf("j=%d channel=%d sum = %u sumsq=%u\n",j,chn,(uint32_t)sum[chn],(uint32_t)sumsq[chn]);
    }
  }
  for(chn=0;chn<NCHANNELS;chn++){
    xprintf("Channel %d Average %u min %u max %u \n",chn,(uint32_t)(sum[chn]/(MEASUREMENT_CYCLES)),min[chn],max[chn]);
    xprintf("Square of sum %lu\n",sum[chn]*sum[chn]);
    xprintf("N^2*sigma^2 = %u\n", (uint32_t)(sumsq[chn]*MEASUREMENT_CYCLES-sum[chn]*sum[chn]));
    xprintf("uint_sqrt(%u)=%u\n",(uint32_t)(sumsq[chn]*MEASUREMENT_CYCLES-sum[chn]*sum[chn]),uint_sqrt((uint32_t)(sumsq[chn]*MEASUREMENT_CYCLES-sum[chn]*sum[chn])));
    xprintf("sigma=%u\n",uint_sqrt((uint32_t)(sumsq[chn]*MEASUREMENT_CYCLES-sum[chn]*sum[chn]))/(MEASUREMENT_CYCLES));
  }
}  


enum state_t {ST_NORMAL, ST_MEASURE, ST_SET, ST_COMMAND_LOOP, ST_PRESSED} state;

int main(void) {
  xfunc_in = usart_getc;
  xfunc_out = usart_putc;
  GPIO_TypeDef *GPIO_port = GPIOC;
  uint16_t GPIO_control_pin = GPIO_Pin_5; 
  state =ST_COMMAND_LOOP;

  if(SysTick_Config(SystemCoreClock/100000)) while(1); // Initialize system timer
  init_adc(operation_channels);
  init_GPIOC_pin();

  usart_open(USART1,9600);
  


  uint8_t ch;
  int32_t selected_channel;
  int32_t threshold[NCHANNELS]={0}; 
  char buffer[INPUT_BUFFER_LEN];
  char *conv_pointer;
  xprintf("Starting conversion\n");
  GPIO_WriteBit(GPIO_port,GPIO_control_pin,0);
  
  while (1) {
    switch(state){
      case ST_PRESSED:
          GPIO_WriteBit(GPIOC,GPIO_Pin_9,1);
          Delay(200000);
          GPIO_WriteBit(GPIOC,GPIO_Pin_9,0);
          state=ST_NORMAL;
          
      case ST_NORMAL:
        selected_channel=normal_operate(threshold);
        if(selected_channel){
          state= ST_PRESSED;
          selected_channel--;
        }

        if(usart_poll_getc(&ch)){
          if(ch == 13 || ch == 10){
            state = ST_COMMAND_LOOP;
          }
          //xprintf("Read character %d",(int32_t)ch);
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
          if(xatoi(&conv_pointer,&selected_channel)){
            xprintf("Setting threshold of %d channel\n",selected_channel);
            if(!xatoi(&conv_pointer,&threshold[selected_channel])){
              threshold[selected_channel]=700;
            }
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

