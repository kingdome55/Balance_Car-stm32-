#include "pwm_test.h"
#include "app_pwm.h"
#include "delay.h"
// @简介：对PWM模块进行测试
// 先使能电机
// 让电机正转30%2秒
// 让电机正转60%2秒
// 让电机正转90%2秒
// 关闭电机

void PWM_Test(void) // main
{
  App_PWM_Init();                 // PWM外设初始化
  App_PWM_Cmd(1);                 // 唤醒电机，使能PWM输出

  App_PWM_Set_L(30);   // 30%正转
  App_PWM_Set_R(30);   // 30%正转

  Delay(2000);         // 延迟2s


  App_PWM_Set_L(60);   // 60%正转
  App_PWM_Set_R(60);   // 60%正转

  Delay(2000);         // 延迟2s


  App_PWM_Set_L(90);   // 90%正转
  App_PWM_Set_R(90);   // 90%正转

  Delay(2000);         // 延迟2s


  App_PWM_Cmd(0);      // 让电机休眠

  while(1)
  {

  }
}
