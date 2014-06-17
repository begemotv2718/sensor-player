#include "usart.h"


void usart_open(USART_TypeDef *usart, int baud_rate) {

  GPIO_InitTypeDef GPIO_init_params;
  GPIO_TypeDef *GPIO_port;
  uint16_t GPIO_in_pin, GPIO_out_pin;

  if(usart==USART1){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_port = GPIOA;
    GPIO_in_pin = GPIO_Pin_10;
    GPIO_out_pin = GPIO_Pin_9;
  }
  GPIO_StructInit(&GPIO_init_params);
  GPIO_init_params.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_init_params.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_init_params.GPIO_Pin = GPIO_out_pin;
  GPIO_Init(GPIO_port,&GPIO_init_params);
  GPIO_init_params.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_init_params.GPIO_Pin = GPIO_in_pin;
  GPIO_Init(GPIO_port, &GPIO_init_params);


  USART_InitTypeDef USART_init_params;
  USART_StructInit(&USART_init_params);
  USART_init_params.USART_BaudRate = baud_rate;
  USART_init_params.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(usart,&USART_init_params);
  USART_Cmd(usart, ENABLE);
}

void usart_close(USART_TypeDef *usart){
  USART_DeInit(usart);
}

void usart_putc(USART_TypeDef *usart, int c){
    while ( USART_GetFlagStatus (usart , USART_FLAG_TXE ) == RESET);
    usart->DR = (c & 0xff);
}

int usart_getc(USART_TypeDef *usart){
  while ( USART_GetFlagStatus (usart , USART_FLAG_RXNE ) == RESET);
  return usart->DR && 0xff;
}

