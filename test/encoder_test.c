#include "encoder_test.h"
#include "app_encoder.h"
#include "app_usart2.h"
#include "delay.h"

void Encoder_Test(void)
{
  App_USART2_Init();
  App_Encoder_Init();

  while(1)
  {
    int64_t pos_l = App_Encoder_GetPos_L();
    int64_t pos_r = App_Encoder_GetPos_R();
    
    My_USART_Printf(USART2, "%d, %d\n", (int32_t)pos_l, (int32_t)pos_r);
    
    Delay(50);
    
  }
}

