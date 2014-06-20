#include "spi.h"

static const uint16_t speeds [] = {
  [SPI_SLOW] = SPI_BaudRatePrescaler_64,
  [SPI_MEDIUM] =SPI_BaudRatePrescaler_8, 
  [SPI_FAST] = SPI_BaudRatePrescaler_2
};

void spiInit(SPI_TypeDef *SPIx){
  SPI_InitTypeDef SPI_init;
  GPIO_InitTypeDef GPIO_init;
  
  SPI_StructInit(&SPI_init);
  GPIO_StructInit(&GPIO_init);

  GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
  
  if(SPIx == SPI1){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA,ENABLE);
    GPIO_init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_init.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_init);
    GPIO_init.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_init.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOA, &GPIO_init);
  } else if(SPIx == SPI2){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
    GPIO_init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_init.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_Init(GPIOA, &GPIO_init);
    GPIO_init.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_init.GPIO_Pin = GPIO_Pin_14;
    GPIO_Init(GPIOB, &GPIO_init);
  } else {
    return;
  } 

  SPI_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_init.SPI_Mode = SPI_Mode_Master;
  SPI_init.SPI_DataSize = SPI_DataSize_8b;
  SPI_init.SPI_CPOL = SPI_CPOL_Low;
  SPI_init.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_init.SPI_NSS = SPI_NSS_Soft;
  SPI_init.SPI_BaudRatePrescaler = speeds[SPI_SLOW];
  SPI_init.SPI_CRCPolynomial = 7;
  SPI_Init(SPIx,&SPI_init);

  SPI_Cmd(SPIx, ENABLE);

}

int spiReadWrite(SPI_TypeDef *SPIx, uint8_t *rbuf, uint8_t *tbuf, int cnt, enum spiSpeed speed){
  SPIx->CR1 = (SPIx->CR1 & -SPI_BaudRatePrescaler_256) | speeds[speed];

  int i;
  for(i =0; i< cnt; i++){
    if(tbuf){
      SPI_I2S_SendData(SPIx,*tbuf++);
    }else{
      SPI_I2S_SendData(SPIx,0xff);
    }
    while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
    if(rbuf){
      *rbuf++ = SPI_I2S_ReceiveData(SPIx);
    }else{
      SPI_I2S_ReceiveData(SPIx);
    }
  }
  return i;
}
int spiReadWrite16(SPI_TypeDef *SPIx, uint16_t *rbuf, uint16_t *tbuf, int cnt, enum spiSpeed speed){
  SPIx->CR1 = (SPIx->CR1 & -SPI_BaudRatePrescaler_256) | speeds[speed];

  int i;
  SPI_DataSizeConfig(SPIx, SPI_DataSize_16b);
  for(i =0; i< cnt; i++){
    if(tbuf){
      SPI_I2S_SendData(SPIx,*tbuf++);
    }else{
      SPI_I2S_SendData(SPIx,0xffff);
    }
    while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
    if(rbuf){
      *rbuf++ = SPI_I2S_ReceiveData(SPIx);
    }else{
      SPI_I2S_ReceiveData(SPIx);
    }
  }
  SPI_DataSizeConfig(SPIx, SPI_DataSize_8b);
  return i;
}
