#include "app_bat.h"
#include "delay.h"
static volatile float vbat = 0.0f;
static void TIM2_TRGO_Init(void);
static void ADC1_Init(void);
static void LED_Init(void);



void App_Bat_Init(void)
{ 
 
  TIM2_TRGO_Init();
  ADC1_Init();
  LED_Init();
  
}

static uint32_t lastTime = 0;
static uint32_t stage = 0;

//
//简介：获取电池的当前电压，单位是V
//
float App_Bat_Get(void)
{
  return vbat;
}

void App_Bat_Proc(void)
{
  if(vbat > 7.9) // 电池满电，点亮3颗
  {
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_6, Bit_SET);
  }
  else if(vbat > 7.4) // 75%，点亮2颗
  {
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_6, Bit_RESET);
  }
  else if(vbat > 7) // 50%，点亮1颗
  {
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_6, Bit_RESET);
  }
  else if(vbat >6.5) // 20%，全部熄灭
  {
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_6, Bit_RESET);
  }
  else
  {
    uint32_t now = GetTick();
    
    if(now - lastTime >100)
  
    switch(stage)
    {
      case 0: // 当前熄灭
      {
        GPIO_WriteBit(GPIOA, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6, Bit_SET); // 点亮
        stage = 1;
        break;
      }
      case 1: // 当前点亮
      {
        GPIO_WriteBit(GPIOA, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6, Bit_RESET); // 熄灭
        stage = 0;
        break;
      }
    }
    lastTime = now;
  }
}

//
//@简介：初始化电源指示灯
//     PA4 PA5 PA6 - Out__PP
//
static void LED_Init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_Init(GPIOA, &GPIO_InitStruct);
}
static void TIM2_TRGO_Init(void)
{
  //#1.初始化TIM2_TRGO,每10ms产生一个脉冲  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
  
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period = 9999;
  TIM_TimeBaseInitStruct.TIM_Prescaler = 71;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
  
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update); //选择主机模式为update模式
  
  TIM_Cmd(TIM2, ENABLE); //闭合时基单元开关
}
static void ADC1_Init(void)
{
  // 对ADC进行初始化
  // 将PB0初始化模拟模式
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 开启GPIOB的时钟

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOB, &GPIO_InitStruct);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // 开启ADC1的时钟

  // 初始化ADC的基本参数
  ADC_InitTypeDef ADC_InitStruct = {0};
  ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStruct.ADC_Mode = DISABLE;
  ADC_InitStruct.ADC_NbrOfChannel = 1;
  ADC_InitStruct.ADC_ScanConvMode = DISABLE;

  ADC_Init(ADC1, &ADC_InitStruct);
 
  // 设置注入序列的参数
  ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T2_TRGO); // 把定时器2的TRGO作为注入序列的外部触发
  ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE); // 闭合外部触发开关
  ADC_InjectedChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_1Cycles5);

  // 配置ADC的JEOC中断
  ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE); // 闭合JEOC的开关

  // NVIC 中断优先级分组 0 1 2 3 4 5
  NVIC_InitTypeDef NVIC_InitStruct = {0};
  NVIC_InitStruct.NVIC_IRQChannel = ADC1_2_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;

  NVIC_Init(&NVIC_InitStruct);

  ADC_Cmd(ADC1, ENABLE); // 闭合ADC的总开关
}



  //
  // @简介：ADC1和ADC2的中断响应函数
  //
  void ADC1_2_IRQHandler(void)
  {
    if(ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC) == SET)
    {
      ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);

      uint16_t jdr1 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);

      vbat = jdr1 / 4095.0f *3.3 * 8.4f / 3.3f;
    }
  }
