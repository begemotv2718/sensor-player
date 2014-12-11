#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"

void usart_open(USART_TypeDef *usart, int baud_rate);
void usart_close(USART_TypeDef *usart);
void usart_putc(USART_TypeDef *usart, int c);
int usart_getc(USART_TypeDef *usart);
void usart_putc_old(USART_TypeDef *usart, int c);
int usart_getc_old(USART_TypeDef *usart);

