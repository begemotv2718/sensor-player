/**
 @file adc.h
 @brief Functions for measuring capacitance with adc

 Functions for measuring capacitance with adc
 @author begemotv2718
 @bugs Will see
*/

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
  /*@name push_pull related parameters*/
  /*@{*/
  GPIO_TypeDef *pp_port;
  uint16_t pp_pin;
  /*@}*/
  /*@name ADC related parameters */
  /*@{*/
  GPIO_TypeDef *adc_port;
  uint16_t adc_pin;
  uint8_t channel;
  /*@}*/
}; 
/**
 @brief Initializes GPIO pins for ADC and push-pull
 
 Initializes GPIO pins for ADC and push-pull
 @param[in] chlist list of struct channel describing pins used
 @returns nothing
*/
void init_adc(struct adc_channel *chlist);
/**
   @brief Setup timers and ADC for conversion
   
   Setup timers and ADC for conversion
   @param[in]  buf_size Size of the conversion buffer
   @param[out] conv_buf Conversion buffer
   @param[in]  channel  Struct adc_channel for the measurement channel
   @param[in]  setup    Function that should be called at first conversion
   @returns    noting
*/
void start_conversion(int buf_size, uint16_t *conv_buf, struct adc_channel *channel, void (*setup)(struct adc_channel *)); 

/**
 @brief returns true if we measured all samples

 @returns 1 if conv is finished 0 otherwise
*/
int conv_finished(void);
#endif
