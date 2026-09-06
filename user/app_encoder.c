 #include "app_encoder.h"

static volatile int64_t encoder_l = 0; // 左电机编码器值
static volatile int64_t encoder_r = 0; // 右电机编码器值
 
static void Encoder_L_Init(void); // 左编码器初始化
static void Encoder_R_Init(void); // 右编码器初始化

//
// @简介：对编码器模块进行初始化
//
void App_Encoder_Init(void)
{
  Encoder_L_Init();
  Encoder_R_Init();
}

//
// @简介：读取左编码器的当前位置
//
int64_t App_Encoder_GetPos_L(void)
{
  return encoder_l;
}

//
// @简介：读取右编码器的当前位置
//
int64_t App_Encoder_GetPos_R(void)
{
  return encoder_r;
}

static void Encoder_L_Init(void)
{
  // 初始化A和B的引脚
  // 关闭JTAG，开启SWD
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  
  // PB14 PB15 - IPU
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;

  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  // EXTI初始化
  // 让EXTI_Line3监控PB3
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3);
  
  // 配置EXTI的参数
  EXTI_InitTypeDef EXTI_InitStruct = {0};

  EXTI_InitStruct.EXTI_Line = EXTI_Line3;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;

  EXTI_Init(&EXTI_InitStruct);
  
  // 开启EXTI的中断
  NVIC_InitTypeDef NVIC_InitStruct = {0};

  NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn; // 中断编号
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

  NVIC_Init(&NVIC_InitStruct);



}

//
// @简介：右编码器初始化
//
static void Encoder_R_Init(void)
{
  // 初始化A和B的引脚
  // PB3，PB4 - IPU
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;

  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  // EXTI初始化
  // 让EXTI_Line3监控PB 14
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
  
  // 配置EXTI的参数
  EXTI_InitTypeDef EXTI_InitStruct = {0};

  EXTI_InitStruct.EXTI_Line = EXTI_Line14;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;

  EXTI_Init(&EXTI_InitStruct);
  
  // 开启EXTI的中断
  NVIC_InitTypeDef NVIC_InitStruct = {0};

  NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn; // 中断编号
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

  NVIC_Init(&NVIC_InitStruct);


}

//
// @简介：EXTI3的中断响应函数，对应左编码器的A相
//
void EXTI3_IRQHandler(void)
{
  EXTI_ClearFlag(EXTI_Line3); // 对中断标志位清零
  uint8_t a = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3); // A相的当前电压
  uint8_t b = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4); // B相的当前电压

  if(a == Bit_SET) // 上升沿
  {
    if(b == Bit_RESET)
    {
      encoder_l++;
    }
    else
    {
      encoder_l--;
    }
  }
  else // 下降沿
  {
    if(b == Bit_RESET)
    {
      encoder_l--;
    }
    else
    {
      encoder_l++;
    }
  }

}

//
// @简介：EXTI15_10的中断响应函数，对应右编码器的A相
//
void EXTI15_10_IRQHandler(void)
{
  if(EXTI_GetFlagStatus(EXTI_Line14) == SET)
  {
    EXTI_ClearFlag(EXTI_Line14); // 对标志位进行清零
    uint8_t a = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14); // A相的当前电压
    uint8_t b = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15); // B相的当前电压

    if(a == Bit_SET) // 上升沿
    {
      if(b == Bit_RESET)
      {
        encoder_r++;
      }
      else
      {
        encoder_r--;
      }
    }
    else // 下降沿
    {
      if(b == Bit_SET)
      {
        encoder_r++;
      }
      else
      {
        encoder_r--;
      }

    }
  }

}

