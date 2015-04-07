#include "dac_play.h"
//DAC Channel1 is either DMA1_Channel3 or DMA2_Channel3
#define DMA_Channel_DAC DMA1_Channel3 
#define DMA_Channel_DAC_IRQn DMA1_Channel3_IRQn 
//DMA2_Channel3_IRQn
#define TIM_DAC TIM2
#define DHR12R1_OFFSET             ((uint32_t)0x00000008)
#define DAC_DHR8R1_Address      0x40007410
#define DAC_DHR12R1_Address     0x40007408

uint16_t *buf;
uint16_t buf_size;
volatile uint16_t cur_addr=0;


void initialize_dac(uint16_t *dac_buf,uint16_t dac_buf_size){
  GPIO_InitTypeDef GPIO_InitStr;
  DAC_InitTypeDef DAC_InitStr;
  DMA_InitTypeDef DMA_InitStr;
  NVIC_InitTypeDef NVIC_InitStr;
  


  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);

  GPIO_StructInit(&GPIO_InitStr);
  GPIO_InitStr.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStr.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStr);

  // DAC channel 1 Configuration
  DAC_StructInit(&DAC_InitStr);
  DAC_InitStr.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_InitStr.DAC_Trigger = DAC_Trigger_T2_TRGO;
  DAC_Init(DAC_Channel_1,&DAC_InitStr);

  RCC_AHBPeriphClockCmd (RCC_AHBPeriph_DMA1,ENABLE);

  DMA_DeInit(DMA_Channel_DAC);
  DMA_InitStr.DMA_PeripheralBaseAddr = DAC_DHR12R1_Address;//(uint32_t)DAC_BASE+DHR12R1_OFFSET; //??
  DMA_InitStr.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStr.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStr.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStr.DMA_BufferSize = dac_buf_size;
  DMA_InitStr.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStr.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStr.DMA_M2M = DMA_M2M_Disable;
  
  DMA_InitStr.DMA_MemoryBaseAddr = (uint32_t)dac_buf;
  DMA_InitStr.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStr.DMA_DIR = DMA_DIR_PeripheralDST;

  DMA_Init(DMA_Channel_DAC,&DMA_InitStr);

  DMA_ITConfig(DMA_Channel_DAC, DMA_IT_TC | DMA_IT_HT, ENABLE);

  NVIC_InitStr.NVIC_IRQChannel = DMA_Channel_DAC_IRQn; //???
  NVIC_InitStr.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStr.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStr.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStr);


#if 0  
  buf=dac_buf;
  buf_size = dac_buf_size;
  NVIC_InitStr.NVIC_IRQChannel = TIM2_IRQn; 
  NVIC_InitStr.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStr.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStr.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStr);

#endif  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseInitTypeDef TIM_base_init;
  TIM_TimeBaseStructInit(&TIM_base_init);
  TIM_base_init.TIM_Prescaler = SystemCoreClock/64000 - 1;
  TIM_base_init.TIM_Period = 2-1;
  TIM_base_init.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM_DAC,&TIM_base_init);

  TIM_SelectOutputTrigger(TIM_DAC,TIM_TRGOSource_Update);

  // Enable all
}


void start_dac(void){
  //TIM_ITConfig(TIM_DAC,TIM_IT_Update, ENABLE);
  DMA_Cmd(DMA_Channel_DAC/*???*/, ENABLE);
  DAC_Cmd(DAC_Channel_1,ENABLE);
  DAC_DMACmd(DAC_Channel_1,ENABLE);
  TIM_Cmd(TIM_DAC,ENABLE);
}

void stop_dac(void){
  TIM_Cmd(TIM_DAC,DISABLE);
  DAC_DMACmd(DAC_Channel_1,DISABLE);
  DAC_Cmd(DAC_Channel_1,DISABLE);
  DMA_Cmd(DMA_Channel_DAC/*???*/, DISABLE);
 // TIM_ITConfig(TIM_DAC,TIM_IT_Update, DISABLE);
}
#if 0
void TIM2_IRQHandler(void){
  DAC_SetChannel1Data(DAC_Align_12b_R,buf[cur_addr]);
  cur_addr++;
  if(cur_addr>=buf_size){
    cur_addr=0;
  }
  TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
}
#endif
