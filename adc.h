#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_conf.h"
#include <misc.h>

void init_adc(void);
void start_conversion(int buf_size, uint16_t *conv_buf, void (*setup)(void));
int conv_finished(void);
