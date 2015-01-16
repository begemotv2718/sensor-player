#include "stm32f10x.h"
uint32_t uint_sqrt(uint32_t y){
  uint32_t res;
  int32_t i;
  res=1;
  for(i=0;i<20;i++){
    res=(res+y/res)/2;
  }
  return res;
}
