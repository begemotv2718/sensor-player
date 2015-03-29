#ifndef ADC_H
#define ADC_H
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_conf.h"
#include <misc.h>
#include <stdlib.h>

struct adc_channel {
  GPIO_TypeDef *pp_port;
  uint16_t pp_pin;
  GPIO_TypeDef *adc_port;
  uint16_t adc_pin;
  uint8_t channel;
}; 

void init_adc(struct adc_channel *chlist);
void start_conversion(int buf_size, uint16_t *conv_buf, struct adc_channel *channel, void (*setup)(struct adc_channel *)); 
int conv_finished(void);
#endif
