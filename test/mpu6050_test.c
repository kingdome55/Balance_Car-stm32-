#include "mpu6050_test.h"
#include "app_usart2.h"
#include "app_mpu6050.h"
#include "delay.h"

void MPU6050_Test(void)
{
  App_USART2_Init();
  App_MPU6050_Init();

  while(1)
  {
    App_MPU6050_Update();

    float ax = App_MPU6050_GetAx();
    float ay = App_MPU6050_GetAy();
    float az = App_MPU6050_GetAz();

    float temperature = App_MPU6050_Temperature();

    float gx = App_MPU6050_GetGx();
    float gy = App_MPU6050_GetGy();
    float gz = App_MPU6050_GetGz();

    My_USART_Printf(USART2, "%f,%f,%f,%f,%f,%f,%f\n", ax, ay, az, temperature, gx, gy, gz);
    Delay(10);
  }
}

static void USART2_Proc(void);

//
//@简介：用来测试欧拉角（VOFA）
//
void MPU6050_EularAngle_Test(void) // main
{
  App_MPU6050_Init();
  App_USART2_Init();
  USART2_Proc();
  
  while(1)
  {
   App_MPU6050_Proc();
  }
}

//@简介：每间隔10ms把欧拉角的计算结果发给电脑
//
static void USART2_Proc()
{
  static uint32_t nex = 0;
  
  if(GetTick() < nex) return;
  nex += 10;
  float ax = App_MPU6050_GetAx();
  float ay = App_MPU6050_GetAy();
  float az = App_MPU6050_GetAz();

  float temperature = App_MPU6050_Temperature();

  float gx = App_MPU6050_GetGx();
  float gy = App_MPU6050_GetGy();
  float gz = App_MPU6050_GetGz();
  
  float yaw   = App_MPU6050_GetYaw();
  float pitch = App_MPU6050_GetPitch();
  float roll  = App_MPU6050_GetRoll();

  My_USART_Printf(USART2,"%f,%f,%f,%f,%f,%f,%f,%f,%f\n",ax,ay,az,gx,gy,gz,yaw,pitch,roll);

}
