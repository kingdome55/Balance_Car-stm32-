#include "app_button.h"
#include "button.h"
#include "app_pwm.h"
static Button_TypeDef userKey;// 用户按钮
static void OnuserKey_Clicked(uint8_t clicks);

void App_Button_Init()
{
  Button_InitTypeDef Button_InitStruct = {0};
  
  Button_InitStruct.GPIOx = GPIOA;
  Button_InitStruct.GPIO_Pin = GPIO_Pin_11;
  
  My_Button_Init(&userKey, &Button_InitStruct);
  
  My_Button_SetClickCb(&userKey, OnuserKey_Clicked);
  
}

static uint8_t pwm_on = 0; //0-休眠 1-活动

void App_Button_Proc()
{
  My_Button_Proc(&userKey);
  
}

static void OnuserKey_Clicked(uint8_t clicks)
{
  if(clicks == 1)
  {
    if(pwm_on == 0)
    {
      pwm_on = 1;
    }
    else
    {
      pwm_on = 0;
    }
    App_PWM_Cmd(pwm_on);
  }
}
