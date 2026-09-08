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
    float pos_l = App_Encoder_GetPos_L();
    float pos_r = App_Encoder_GetPos_R();
    
    My_USART_Printf(USART2, "%f, %f\n", pos_l,pos_r);
    
    Delay(50);
    
  }
}

static float last_pos_l = 0.0f;
static float last_pos_r = 0.0f;
//
//@简介：M法测速测试
//
void Encoder_M_Method_Test(void)
{
  App_Encoder_Init();
  App_USART2_Init();
  while(1)
  {
    Delay(1);
    
    float pos_l = App_Encoder_GetPos_L();
    float pos_r = App_Encoder_GetPos_R();
    
    float M_l = pos_l - last_pos_l;
    float M_r = pos_r - last_pos_r;
    
    float omega_l = M_l / 0.001f;
    float omega_r = M_r / 0.001f;
    
    My_USART_Printf(USART2, "%f, %f, %f, %f\n", pos_l, pos_r, omega_l, omega_r);
    
    last_pos_l = pos_l;
    last_pos_r = pos_r;
  }
}
//
//@简介：T法测试测试
//
void Encoder_T_Method_Test(void)
{
  App_USART2_Init();
  App_Encoder_Init();
  while(1)
  {
    Delay(1);
    
    // T 法测速应读取瞬时角速度接口 GetSpeed，而不是位置接口 GetPos
    float omega_l = App_Encoder_GetSpeed_L();
    float omega_r = App_Encoder_GetSpeed_R();
    
    My_USART_Printf(USART2, "%f, %f\n", omega_l, omega_r);
  }
  
}
