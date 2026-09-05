#include "app_pwm.h"

static void STBY_Pin_Init(void);
static void Motor_L_Init(void);//左电机初始化
static void Motor_R_Init(void);//右电机初始化


//
// @简介：对TB6612进行初始化
//
void App_PWM_Init(void)
{
  STBY_Pin_Init(); // STBY引脚初始化
  Motor_L_Init(); //左电机初始化
  Motor_R_Init();//右电机初始化 
}

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

#include "math.h"
//
// @简介：设置左电机的占空比
// @参数：duty - 占空比的具体值，范围-100.0f ~ +100.0f
//
void App_PWM_Set_L(float Duty)
{
  float sign; // 符号，正数 - +1，负数 - -1

  if(Duty >= 0) sign = 1;
  else sign = -1;

  Duty = fabsf(Duty);

  if(sign < 0) // 反转
  {
    GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);   // AIN1 - 高
    GPIO_WriteBit(GPIOA, GPIO_Pin_10, Bit_RESET); // AIN2 - 低
  }
  else
  {
    GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);  // AIN1 - 低
    GPIO_WriteBit(GPIOA, GPIO_Pin_10, Bit_SET);   // AIN2 - 高
  }

  uint16_t ccr = Duty / 100.0f * 999;

  TIM_SetCompare1(TIM1, ccr);
}


//
// @简介：设置右电机的占空比
//@参数：duty - 占空比的具体值，范围-100.0f ~ +100.0f
//
void App_PWM_Set_R(float Duty)
{
  float sign; // 符号，正数 - +1，负数 - -1

  if(Duty >= 0) sign = 1;
  else sign = -1;

  Duty = fabsf(Duty);

  if(sign >= 0) // 正转
  {
    GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);   // BIN1 - 高
    GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET); // BIN2 - 低
  }
  else
  {
    GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);  // BIN1 - 低
    GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET);   // BIN2 - 高
  }

  uint16_t ccr = Duty / 100.0f * 999;

  TIM_SetCompare1(TIM4, ccr);

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
// 
//@简介：左电机初始化
//       AIN1 - PA9 - Out_pp
//       AIN2 - PA10  - Out_pp
static void Motor_L_Init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  // 初始化PWM
  // 初始化PA8 - AF_PP
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_Init(GPIOA, &GPIO_InitStruct);

  // 对定时器1进行初始化
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); // 开启定时器1的时钟

  // 设置时基单元的参数
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};

  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period = 999;
  TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
  
    // 配置输出比较
  TIM_OCInitTypeDef TIM_OCInitStruct = {0};

  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStruct.TIM_OutputState = ENABLE;
  TIM_OCInitStruct.TIM_Pulse = 0;

  TIM_OC1Init(TIM1, &TIM_OCInitStruct);

  // 配置MOE的开关
  TIM_CtrlPWMOutputs(TIM1, ENABLE);

  // 闭合定时器的总开关
  TIM_Cmd(TIM1, ENABLE);
 
}
//
// @简介：右电机初始化
//        BIN1 - PB5 - Out_PP
//        BIN2 - PB7 - Out_PP
//
static void Motor_R_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // 初始化BIN1和BIN2
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  // 初始化PWM
  // 初始化PB6 - AF_PP
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  // 对定时器4进行初始化
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); // 开启定时器1的时钟

  // 设置时基单元的参数
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};

  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period = 999;
  TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
  
  // 配置输出比较
  TIM_OCInitTypeDef TIM_OCInitStruct = {0};

  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStruct.TIM_OutputState = ENABLE;
  TIM_OCInitStruct.TIM_Pulse = 0;

  TIM_OC1Init(TIM4, &TIM_OCInitStruct);

  // 配置MOE的开关
  TIM_CtrlPWMOutputs(TIM4, ENABLE);

  // 闭合定时器的总开关
  TIM_Cmd(TIM4, ENABLE);

}

