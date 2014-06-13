/**
  @file    main.c
  @author  h0rr0rr_drag0n
  @version V0.0.1
  @date    22-Aug-2011
  @brief   Template of Linux project for STM32VLDiscovery
**/

#include "stm32f10x.h"

/**
   @brief  Enable clocking on various system peripheral devices
   @params None
   @retval None
**/

enum STATES { STATE_idle, STATE_btn_pressed_1, STATE_btn_pressed_2, STATE_btn_released };

void initialize_GPIOA() {
  /* enable clocking on Port C */
  GPIO_InitTypeDef GPIOA_init_params;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIOA_init_params.GPIO_Speed = GPIO_Speed_2MHz;
  GPIOA_init_params.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIOA_init_params);

}

void initialize_GPIOC() {
  /* enable clocking on Port C */
  GPIO_InitTypeDef GPIOC_init_params;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  /* Blue LED sits on PC[8] and Green LED sits on PC[9]*/
  GPIOC_init_params.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  /* Output maximum frequency selection is 10 MHz.
     Do not worry that internal oscillator of STM32F100RB
     works on 8MHz frequency - Cortex-M3 core has a various
     facilities to carefully tune the frequency for almost
     peripheral devices.
  */
  GPIOC_init_params.GPIO_Speed = GPIO_Speed_2MHz;
  /* Push-pull output.
     Двухтактный выход.
  */
  GPIOC_init_params.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIOC_init_params);
}


int main(void) {
  uint8_t state_pin_9 =1 ;
  enum STATES curr_state = STATE_idle;

  initialize_GPIOA();
  initialize_GPIOC();

  


  /* Set pins 8 and 9 at PORTC to high level */
  GPIO_SetBits(GPIOC, GPIO_Pin_8);
  GPIO_WriteBit(GPIOC, GPIO_Pin_9, state_pin_9);
  
  while (1) {
    switch(curr_state) {
         case STATE_idle: 
             if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==Bit_SET){
               curr_state=STATE_btn_pressed_1;
             }
             break;
         case STATE_btn_pressed_1:
            if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==Bit_SET){
               curr_state = STATE_btn_pressed_2;
            }else{
               curr_state = STATE_idle;
            }
            break;
        case STATE_btn_pressed_2:
           if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==Bit_RESET){
             curr_state = STATE_btn_released;
           }
           break;
        case STATE_btn_released:
           state_pin_9 = 1-state_pin_9;
           GPIO_WriteBit(GPIOC, GPIO_Pin_9, state_pin_9);
           curr_state = STATE_idle;
           break;
    } 
  }
}
