#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "usart.h"
#include "queue.h"

struct Queue UART_TXq, UART_RXq;

volatile int TxReenable =0;
volatile int RxOverflow =0;

void USART1_IRQHandler(void){
  USARTx_IRQHandler(USART1);
}

void USARTx_IRQHandler(USART_TypeDef *usart){
  if(USART_GetITStatus(usart,USART_IT_RXNE) != RESET){
    uint8_t data;
    data = USART_ReceiveData(usart) & 0xff;
    if(!Enqueue(&UART_RXq,data)){
      RxOverflow = 1;
    }
  }
  
  if(USART_GetITStatus(usart, USART_IT_TXE) !=RESET){
    uint8_t data;
    if(Dequeue(&UART_TXq,&data)){
       USART_SendData(USART1, data);
    }else{
      TxReenable = 0;
    }
  }
}


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

  usart_init_interrupt(usart);
}

void usart_close(USART_TypeDef *usart){
  USART_DeInit(usart);
  USART_ITConfig(usart,USART_IT_RXE, DISABLE);
  USART_ITConfig(usart,USART_IT_TXE, DISABLE);
}

void usart_init_interrupt(USART_TypeDef *usart){
  NVIC_InitTypeDef NVIC_InitStruct;
  if(usart==USART1){
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
  }else if(usart == USART2){
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
  }
  NVIC_InitStruct.NVIC_IRQChannelSubPriority =3;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  USART_ITConfig(usart,USART_IT_RXE, ENABLE);
}

int usart_getc(USART_TypeDef *usart){
  uint8_t data;
  while(!Dequeue(&UART_RXq,&data)){
    return data;
  }
}

int usart_putc(USART_TypeDef *usart, int c){
  while(! Enqueue(&UART_TXq,c));
  if(!TxReenable){
    TxReenable = 1;
    USART_ITConfig(usart, USART_IT_TXE, ENABLE);
  }
}


/*
void usart_putc(USART_TypeDef *usart, int c){
    while ( USART_GetFlagStatus (usart , USART_FLAG_TXE ) == RESET);
    usart->DR = (c & 0xff);
}

int usart_getc(USART_TypeDef *usart){
  while ( USART_GetFlagStatus (usart , USART_FLAG_RXNE ) == RESET);
  return usart->DR && 0xff;
}
*/
