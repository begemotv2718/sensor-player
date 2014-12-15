#include "adc.h"

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
}  


