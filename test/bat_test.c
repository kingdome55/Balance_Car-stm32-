#include "app_usart2.h"
#include "app_bat.h"
#include "delay.h"
#include "bat_test.h"
//
// @简介：电池电压监控模块的测试程序
//        通过串口2把电压发送给电脑
//        用vofa显示曲线
//

void Bat_Test(void) // main
{
  App_USART2_Init(); // 初始化串口2（用来打印数据）
  App_Bat_Init(); // 电压监控模块初始化

  while(1)
  {
    // 每间隔10ms显示一个数据点
    float volt = App_Bat_Get();

    My_USART_Printf(USART2, "%.3f\n", volt);

    Delay(10);
  }
}
