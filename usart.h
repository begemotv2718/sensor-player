#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"

int usart_open(USART_TypeDef *usart, int baud_rate);
void usart_close();
void usart_putc(int c);
int usart_getc();

