#include "adc.h"

static uint16_t *conv_buffer;
static int buffer_size;
static void (*setup_conversion)(struct adc_channel *channel); 

volatile int conversions_done;


struct adc_channel *adc_ch;

void init_tim3(void){
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  TIM_TimeBaseInitTypeDef TIM_base_init;
  TIM_TimeBaseStructInit(&TIM_base_init);
  TIM_base_init.TIM_Prescaler = SystemCoreClock/100000 - 1;
  TIM_base_init.TIM_Period = 2-1;
  TIM_base_init.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3,&TIM_base_init);

  TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update);

}

void init_adc(struct adc_channel *chlist){
  /*Currently configure port C, ADC channel 10 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

  while(chlist->adc_port !=NULL){
    GPIO_InitTypeDef GPIO_init_params;
    GPIO_StructInit(&GPIO_init_params);

    GPIO_init_params.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_init_params.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_init_params.GPIO_Pin = chlist->adc_pin;
    GPIO_Init(chlist->adc_port, &GPIO_init_params);

    

    GPIO_StructInit(&GPIO_init_params);
    GPIO_init_params.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_init_params.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_init_params.GPIO_Pin = chlist->pp_pin;
    GPIO_Init(chlist->pp_port, &GPIO_init_params);

  }
  ADC_InitTypeDef ADC_config;

  ADC_config.ADC_Mode = ADC_Mode_Independent;
  ADC_config.ADC_ScanConvMode = DISABLE;
  ADC_config.ADC_ContinuousConvMode = ENABLE;
  ADC_config.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_config.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_config.ADC_NbrOfChannel = 1;



  ADC_Init(ADC1, &ADC_config);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_28Cycles5);
  ADC_Cmd(ADC1, ENABLE);
  while(ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));
  ADC_Cmd(ADC1, DISABLE);

  init_tim3();

  NVIC_InitTypeDef NVIC_InitStruct;
  NVIC_InitStruct.NVIC_IRQChannel = ADC1_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority =3;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  ADC_config.ADC_Mode = ADC_Mode_Independent;
  ADC_config.ADC_ScanConvMode = DISABLE;
  ADC_config.ADC_ContinuousConvMode = DISABLE;
  ADC_config.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
  ADC_config.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_config.ADC_NbrOfChannel = 1;

  ADC_Init(ADC1, &ADC_config);

}  


void ADC1_IRQHandler(void){
  if(!setup_conversion){
    ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
    return;
  }
  if(conversions_done ==0){
    //call preparation function
    setup_conversion(adc_ch);
    conv_buffer[conversions_done]=ADC1->DR;
  }else if(conversions_done<buffer_size){
    uint16_t ain;
    ain = ADC1->DR;
    conv_buffer[conversions_done]=ain;
  }else{
    //Stop timer
    TIM_Cmd(TIM3,DISABLE);
    ADC_ITConfig(ADC1,ADC_IT_EOC, DISABLE);
  }

  conversions_done++;
  ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
}

int conv_finished(void){
  return conversions_done>=buffer_size;
}

void start_conversion(int buf_size, uint16_t *conv_buf, struct adc_channel *channel, void (*setup)(struct adc_channel *)){  
  conv_buffer=conv_buf;
  buffer_size = buf_size;
  setup_conversion=setup;
  conversions_done=0;
  adc_ch=channel;

  ADC_RegularChannelConfig(ADC1, channel->channel, 1, ADC_SampleTime_28Cycles5);

 // ADC_ITConfig(ADC1,ADC_IT_EOC, ENABLE);
  ADC_ExternalTrigConvCmd(ADC1,ENABLE);

  ADC_Cmd(ADC1, ENABLE);
  ADC_ITConfig(ADC1,ADC_IT_EOC, ENABLE);
  TIM_Cmd(TIM3,ENABLE);
}
