#ifndef DAC_PLAY_H
#define DAC_PLAY_H
#include "stm32f10x.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "misc.h"

void initialize_dac(uint16_t *dac_buf,uint16_t dac_buf_size);
void start_dac(void);
void stop_dac(void);


#endif
