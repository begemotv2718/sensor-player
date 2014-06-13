/**
  @file    main.c
  @author  h0rr0rr_drag0n
  @version V0.0.1
  @date    22-Aug-2011
  @brief   Template of Linux project for STM32VLDiscovery
**/

#include "stm32f10x.h"


volatile uint32_t TimingDelay;

void Delay(uint32_t delay){
  TimingDelay = delay;
  while(TimingDelay !=0);
}

void SysTick_Handler (void){
  if ( TimingDelay != 0x00)
    TimingDelay --;
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


  if(SysTick_Config(SystemCoreClock/1000)) while(1); // Initialize system timer
  initialize_GPIOC();

  


  /* Set pins 8 and 9 at PORTC to high level */
  GPIO_SetBits(GPIOC, GPIO_Pin_8);
  
  while (1) {
    GPIO_WriteBit(GPIOC, GPIO_Pin_9, (state_pin_9)? Bit_SET: Bit_RESET);
    state_pin_9 = 1-state_pin_9;
    Delay(100);
  }
}

