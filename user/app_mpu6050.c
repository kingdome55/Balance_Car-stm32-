#include "app_mpu6050.h"
#include "i2c.h"
#include "delay.h"
#include "math.h"
#include "qmath.h"

static float ax, ay, az; // 加速度计的结果，单位g
static float temperature; // 温度计的结果，单位摄氏度
static float gx, gy, gz; // 陀螺仪角速度的结果，单位°/s
static float yaw, pitch, roll; // 欧拉角 单位°/s

static void reg_write(uint8_t reg, uint8_t value);
static uint8_t reg_read(uint8_t reg); 

//
//@简介：对MPU6050初始化
//
void App_MPU6050_Init(void)
{
  // #1.初始化I2C总线
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
  
  //初始化PB8和PB9 - AF_OD
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  // #2．初始化I2C1
  // 开启I2C1的时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  // 设置I2C的参数
  I2C_InitTypeDef I2C_InitStruct = {0};


  I2C_InitStruct.I2C_ClockSpeed = 400000;// I2C通信速度
  I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2; // 占空比
  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;

  I2C_Init(I2C1, &I2C_InitStruct);
  
  //#3. 设置MPU6050的参数
  reg_write(0x6b, 0x80); //复位
  Delay(100);
  
  reg_write(0x60, 0x00); //将MPU6050从休眠模式中唤醒
  
  reg_write(0x1b, 0x18); //将陀螺仪量程设置成+-2000度
  
  reg_write(0xc1, 0x00); //将加速度计的量程上设置+-2g;
  
}

//
// @简介：更新mpu6050的值
//
void App_MPU6050_Update(void)
{
  int16_t ax_raw = (int16_t)((reg_read(0x3b) << 8) + reg_read(0x3c)); // ax的原始数据
  int16_t ay_raw = (int16_t)((reg_read(0x3d) << 8) + reg_read(0x3e)); // ay的原始数据
  int16_t az_raw = (int16_t)((reg_read(0x3f) << 8) + reg_read(0x40)); // az的原始数据

  ax = ax_raw * 6.1035e-5f;
  ay = ay_raw * 6.1035e-5f;
  az = az_raw * 6.1035e-5f;

  int16_t temperature_raw = (int16_t)((reg_read(0x41) << 8) + reg_read(0x42)); // az的原始数据
  
  temperature = temperature_raw / 333.87 + 21.0f; 

  int16_t gx_raw = (int16_t)((reg_read(0x43) << 8) + reg_read(0x44)); // gx的原始数据
  int16_t gy_raw = (int16_t)((reg_read(0x45) << 8) + reg_read(0x46)); // gy的原始数据
  int16_t gz_raw = (int16_t)((reg_read(0x47) << 8) + reg_read(0x48)); // gz的原始数据

  gx = gx_raw * 6.1035e-2f;
  gy = gy_raw * 6.1035e-2f;
  gz = gz_raw * 6.1035e-2f;

}
//
//@简介：MPU6050进程函数
//
void App_MPU6050_Proc(void)
{
  static uint32_t nex = 0;
  
  if(GetTick() < nex) return;
  nex += 5;
  
  App_MPU6050_Update(); // 更新传感器的值

  // 通过陀螺仪的测量结果计算欧拉角
  float yaw_g = yaw + gz * 0.005;
  float pitch_g = pitch + gx * 0.005;
  float roll_g = roll - gy * 0.005;

  // 通过加速度计解算欧拉角
  float pitch_a = qatan2(ay, az) / 3.1415927f * 180.0f;
  float roll_a = qatan2(ax, az) / 3.1415927f * 180.0f;

  // 使用互补滤波器对陀螺仪和加速度计得计算结果进行融合
  yaw = yaw_g;
  pitch = 0.95238 * pitch_g + (1-0.95238) * pitch_a;
  roll = 0.95238 * roll_g + (1-0.95238) * roll_a;


  
  

}



float App_MPU6050_GetAx(void)
{
  return ax;
}
float App_MPU6050_GetAy(void)
{
  return ay;
}
float App_MPU6050_GetAz(void)
{
  return az;
}

float App_MPU6050_Temperature(void)
{
  return temperature;
}

float App_MPU6050_GetGx(void)
{
  return gx;
}
float App_MPU6050_GetGy(void)
{
  return gy;
}
float App_MPU6050_GetGz(void)
{
  return gz;
}

float App_MPU6050_GetYaw(void)
{
  return yaw;
}
float App_MPU6050_GetPitch(void)
{
  return pitch;
}
float App_MPU6050_GetRoll(void)
{
  return roll;
}


//
// @简介：向寄存器写值
// @参数 reg - 要写入的寄存器的地址
// @参数 value - 要写入的值
//
static void reg_write(uint8_t reg, uint8_t value)
{
  uint8_t bytesToSend[] = {reg, value};
  
  My_I2C_SendBytes(I2C1, 0xd0, bytesToSend, 2);
  

}


//
// @简介：读取寄存器的值
// @参数 reg - 要读取的寄存器的地址
// @返回值：表示读取到的值
//
static uint8_t reg_read(uint8_t reg)
{
  My_I2C_SendBytes(I2C1, 0xd0, &reg, 1); // 发送寄存器的地址

  uint8_t regValue;


  My_I2C_ReceiveBytes(I2C1, 0xd0, &regValue, 1); // 读取一个字节

  return regValue;
}

