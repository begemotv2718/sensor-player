#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"

enum spiSpeed { SPI_SLOW , SPI_MEDIUM , SPI_FAST };

void spiInit(SPI_TypeDef *SPIx);
int spiReadWrite(SPI_TypeDef *SPIx, uint8_t *rbuf, uint8_t *tbuf, int cnt, enum spiSpeed speed);
int spiReadWrite16(SPI_TypeDef *SPIx, uint16_t *rbuf, uint16_t *tbuf, int cnt, enum spiSpeed speed);
