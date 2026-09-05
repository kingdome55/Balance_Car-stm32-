#include "app_pwm.h"

static void STBY_Pin_Init(void);

//
// @简介：控制TB6612进入休眠状态或者活动状态
// @参数：on    0 - 休眠状态，向STBY写L
//            非零 - 活动状态，向STBY写H
//
void App_PWM_Cmd(uint8_t on)
{
  if(on == 0)
  {
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET); // 休眠
  }
  else
  {
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET); // 休眠
  }
}


//
// @简介：对TB6612进行初始化
//
void App_PWM_Init(void)
{
  STBY_Pin_Init(); // STBY引脚初始化
}

//
// @简介：初始化STBY引脚
//        PA1 - Out_PP
//
static void STBY_Pin_Init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA的时钟

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_Init(GPIOA, &GPIO_InitStruct);
}
