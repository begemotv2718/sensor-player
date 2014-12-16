#include "adc.h"

uint16_t *conv_buffer;
int buffer_size;
void (*setup_conversion)(void); 

volatile int conversions_done;

void init_tim3(void){
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  TIM_TimeBaseInitTypeDef TIM_base_init;
  TIM_TimeBaseStructInit(&TIM_base_init);
  TIM_base_init.TIM_Prescaler = SystemCoreClock/100000 - 1;
  TIM_base_init.TIM_Period = 2-1;
  TIM_base_init.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3,&TIM_base_init);

}

void init_adc(void){
  /*Currently configure port C, ADC channel 10 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitTypeDef GPIO_init_params;
  GPIO_StructInit(&GPIO_init_params);
  GPIO_TypeDef *GPIO_port;
  uint16_t GPIO_adc_pin;
  GPIO_port = GPIOA;
  GPIO_adc_pin = GPIO_Pin_6; 


  GPIO_init_params.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_init_params.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_init_params.GPIO_Pin = GPIO_adc_pin;
  GPIO_Init(GPIO_port, &GPIO_init_params);

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

  ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_28Cycles5);

  ADC_ITConfig(ADC1,ADC_IT_EOC, ENABLE);
  ADC_ExternalTrigConvCmd(ADC1,ENABLE);

  ADC_Cmd(ADC1, ENABLE);
}  


void ADC1_IRQHandler(void){
  if(conversions_done ==0){
    //call preparation function
    setup_conversion();
  }else if(conversions_done<buffer_size){
    uint16_t ain;
    ain = ADC1->DR;
    conv_buffer[conversions_done]=ain;
  }else{
    //Stop timer
    TIM_Cmd(TIM3,DISABLE);
  }

  conversions_done++;
  ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
}

int conv_finished(void){
  return conversions_done>=buffer_size;
}

void start_conversion(int buf_size, uint16_t *conv_buf, void (*setup)(void)){  
  conv_buffer=conv_buf;
  buffer_size = buf_size;
  setup_conversion=setup;
  conversions_done=0;

  TIM_Cmd(TIM3,ENABLE);
}
