#include "app_encoder.h"
#include "delay.h"

static volatile int64_t encoder_l = 0; // 左电机编码器值
static volatile int64_t encoder_r = 0; // 右电机编码器值
static volatile int8_t direction_l = 1; // 左电机旋转方向， 1 -正转， -1 - 反转
static volatile int8_t direction_r = 1; // 右电机旋转方向， 1 -正转， -1 - 反转
static volatile uint64_t t0_l = 0, t1_l = 0; // 左电机编码器发生变化的时间，单位 ms
static volatile uint64_t t0_r = 0, t1_r = 0; // 右电机编码器发生变化的时间，单位 ms

 
static void Encoder_L_Init(void); // 左编码器初始化
static void Encoder_R_Init(void); // 右编码器初始化


//
// @简介：读取左轮胎旋转的角速度，omega的值，单位是 度/s
//
float App_Encoder_GetSpeed_L(void)
{
  __disable_irq(); // 关闭单片机总中断
  
  int8_t direction_cpy = direction_l;
  uint64_t t0_cpy = t0_l;
  uint64_t t1_cpy = t1_l;
  
  __enable_irq(); // 开启单片机总中断
  
  if(direction_cpy == +2 || direction_cpy == -2)
  {
    return 0;
  }
  uint64_t now = GetUs();
  
  // 取相邻两个脉冲间隔与上次脉冲至今时间中较小者作为周期（保守估计），保证 T 必定初始化且不为 0
  uint64_t dt = (t0_cpy - t1_cpy) < (now - t0_cpy) ? (t0_cpy - t1_cpy) : (now - t0_cpy);
  if(dt == 0)
  {
    return 0;
  }
  float T = dt * 1.0e-6f;
  
  return (float)direction_cpy / T / 22.0f / (30613.0f / 1500.0f) * 360.0f;
}

//
// @简介：读取右轮胎旋转的角速度，omega的值，单位是 度/s
//
float App_Encoder_GetSpeed_R(void)
{
  __disable_irq(); // 关闭单片机总中断
  
  // 修复复制粘贴笔误：此前误用 direction_l / now - t0_r / 返回 direction_l，导致右轮测速不可靠
  int8_t direction_cpy = direction_r;
  uint64_t t0_cpy = t0_r;
  uint64_t t1_cpy = t1_r;
  
  __enable_irq(); // 开启单片机总中断
  
  if(direction_cpy == +2 || direction_cpy == -2)
  {
    return 0;
  }
  uint64_t now = GetUs();
  
  // 取相邻两个脉冲间隔与上次脉冲至今时间中较小者作为周期（保守估计），保证 T 必定初始化且不为 0
  uint64_t dt = (t0_cpy - t1_cpy) < (now - t0_cpy) ? (t0_cpy - t1_cpy) : (now - t0_cpy);
  if(dt == 0)
  {
    return 0;
  }
  float T = dt * 1.0e-6f;
  
  return (float)direction_cpy / T / 22.0f / (30613.0f / 1500.0f) * 360.0f;
}

//
// @简介：对编码器模块进行初始化
//
void App_Encoder_Init(void)
{
  Encoder_L_Init();
  Encoder_R_Init();
}

//
// @简介：读取左轮胎旋转角度
//
float App_Encoder_GetPos_L(void)
{
  return encoder_l / 22.0f /(30613.0f/ 1500.0f) * 360.0f;
}

//
// @简介：读取右轮胎旋转角度
//
float App_Encoder_GetPos_R(void)
{
  return encoder_r / 22.0f /(30613.0f/ 1500.0f) * 360.0f;
}

static void Encoder_L_Init(void)
{
  // 初始化A和B的引脚
  // 关闭JTAG，开启SWD
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  
  // PB3 PB4 - IPU
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
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
  // PB14，PB15 - IPU
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
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
// @说明：EXTI_Line3 由 Encoder_L_Init() 配置（PB3/PB4），此前误更新到 encoder_r/direction_r，已按其归属改为左编码器变量；若实际硬件 PB3/PB4 接的是右电机，请相应调整归属
//
void EXTI3_IRQHandler(void)
{
  EXTI_ClearFlag(EXTI_Line3); // 对中断标志位清零

  t1_l = t0_l;
  t0_l = GetUs();

  uint8_t a = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3); // A相的当前电压
  uint8_t b = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4); // B相的当前电压

  if((a == Bit_SET && b == Bit_RESET) || (a == Bit_RESET && b == Bit_SET)) // 现在轮胎正转
  {
    encoder_l++;
    if(direction_l < 0) // 之前轮胎是反转
    {
      direction_l = +2;
    }
    else
    {
      direction_l = 1;
    }
  }
  else // 现在轮胎反转
  {
    encoder_l--;
    if(direction_l > 0) // 之前轮胎是正转
    {
      direction_l = -2;
    }
    else
    {
      direction_l = -1;
    }
  }
}


//
// @简介：EXTI15_10的中断响应函数，对应右编码器的A相
// @说明：EXTI_Line14 由 Encoder_R_Init() 配置（PB14/PB15），此前误更新到 encoder_l/direction_l，已按其归属改为右编码器变量；若实际硬件 PB14/PB15 接的是左电机，请相应调整归属
//
void EXTI15_10_IRQHandler(void)
{
  if(EXTI_GetFlagStatus(EXTI_Line14) == SET)
  {
    EXTI_ClearFlag(EXTI_Line14); // 对标志位进行清零
    
    t1_r = t0_r;
    t0_r = GetUs();
    
    uint8_t a = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14); // A相的当前电压
    uint8_t b = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15); // B相的当前电压
    
  if((a == Bit_SET && b == Bit_RESET) || (a == Bit_RESET && b == Bit_SET)) // 现在轮胎反转
  {
    encoder_r--;

    if(direction_r > 0) // 之前轮胎是正转
    {
      direction_r = -2;
    }
    else
    {
      direction_r = -1;
    }
  }
  else // 现在轮胎是正转
  {
    encoder_r++;

    if(direction_r < 0) // 之前轮胎是反转，现在轮胎是正转
    {
      direction_r = +2;
    }
    else
    {
      direction_r = 1;
    }
  }

}

  
}


