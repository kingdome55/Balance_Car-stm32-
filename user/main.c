#include "stm32f10x.h"

#include "app_bat.h"
#include "app_pwm.h"
#include "app_button.h"
#include "app_encoder.h"

#include "pwm_test.h"
#include "bat_test.h"
#include "encoder_test.h"
int main(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  Encoder_T_Method_Test();
  Encoder_M_Method_Test();
//Encoder_Test();
//Bat_Test();
//PWM_Test();
  App_Bat_Init();
  App_Button_Init();
  App_PWM_Init();
  
  
	while(1)
  {
    App_Bat_Proc();
    App_Button_Proc(); // 之前误写为第二个 App_Bat_Proc()，按钮扫描此前一直未被调用
    
	}
}
