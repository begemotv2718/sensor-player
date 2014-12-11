#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "misc.h"
#include "usart.h"
#include "queue.h"

struct Queue UART_TXq, UART_RXq;

volatile int TxReenable =0;
volatile int RxOverflow =0;
static USART_TypeDef *cur_usart;

int usart_getc(){
  uint8_t data;
  while(!Dequeue(&UART_RXq,&data));
  return data;
  
}

void usart_putc(int c){
  while(! Enqueue(&UART_TXq,c));
  if(!TxReenable){
    TxReenable = 1;
    USART_ITConfig(cur_usart, USART_IT_TXE, ENABLE);
  }
}

void USARTx_Handler(){
  uint8_t data;
  if(USART_GetITStatus(cur_usart,USART_IT_RXNE) != RESET){
    data = USART_ReceiveData(cur_usart) & 0xff;
    if(!Enqueue(&UART_RXq,data)){
      RxOverflow = 1;
    }
  }
  
  if(USART_GetITStatus(cur_usart, USART_IT_TXE) !=RESET){
    if(Dequeue(&UART_TXq,&data)){
       USART_SendData(USART1, data);
    }else{
      USART_ITConfig(cur_usart, USART_IT_TXE, DISABLE);
      TxReenable = 0;
    }
  }
}

void USART1_IRQHandler(void){
  USARTx_Handler();
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

  USART_ITConfig(usart,USART_IT_RXNE, ENABLE);
}

int usart_open(USART_TypeDef *usart, int baud_rate) {

  GPIO_InitTypeDef GPIO_init_params;
  GPIO_TypeDef *GPIO_port;
  uint16_t GPIO_in_pin, GPIO_out_pin;
  if(cur_usart){
    return 0;
  }

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
  cur_usart=usart;
  usart_init_interrupt(usart);
  return 1;
}

void usart_close(){
  USART_ITConfig(cur_usart,USART_IT_RXNE, DISABLE);
  USART_ITConfig(cur_usart,USART_IT_TXE, DISABLE);
  USART_DeInit(cur_usart);
  cur_usart=(USART_TypeDef *)0;
}

/*

void usart_putc_old(USART_TypeDef *usart, int c){
    while ( USART_GetFlagStatus (usart , USART_FLAG_TXE ) == RESET);
    usart->DR = (c & 0xff);
}

int usart_getc_old(USART_TypeDef *usart){
  while ( USART_GetFlagStatus (usart , USART_FLAG_RXNE ) == RESET);
  return usart->DR && 0xff;
}
*/
