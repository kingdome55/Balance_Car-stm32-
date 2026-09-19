#include "app_control.h"
#include "pid.h"
#include "delay.h"
#include "app_mpu6050.h"
#include "qmath.h"
#include "app_motor.h"
#include "app_encoder.h"

static PID_TypeDef pid_velocity; // 速度环的PID控制器
static PID_TypeDef pid_theta; // θ环的PID控制器
static PID_TypeDef pid_theta_dot; // θ点环的PID控制器

static const float g = 9.8; //重力加速度 m2s
static const float lp = 0.062; //平衡车轴长 m
static const float rw = 0.032; //轮胎半径 m
  
//
//简介：负责初始化平衡车控制系统
//
void App_Control_Init(void)
{
  PID_Init(&pid_velocity, 10.0f, 1.0f, 0.0f); // 初始化速度环的PID
  PID_LimitConfig(&pid_velocity, +0.5*g, -0.5*g); //将速度环的PID输出限制在+-0.5g
  
  
  PID_Init(&pid_theta, 4.0f, 0.0f, 0.0f); // 初始化θ环PID
  PID_LimitConfig(&pid_theta, +12.57f, -12.57f); // 将θ环的PID控制器输出限制在+-4PI rad/s之间

  PID_Init(&pid_theta_dot, 10.0f, 10.0f, 0.0f); // 初始化θ点环PID
  PID_LimitConfig(&pid_theta_dot, +125.7f, -125.7f); // 将θ点环的PID控制器输出限制在+-40PI rad/s^2之间
}

static float omega_ref = 0.0f;
static uint64_t lastTime = 0;

//
//简介：平衡车控制系统的进程函数，在while循环中调用
//
void App_Control_Proc(void)
{
  static uint32_t nex = 0;
  uint32_t now = GetTick();
  if(now < nex) return;
  nex = now + 5;           //每5ms执行一次
  
  uint64_t mow = GetUs();
  float deltaT = (now - lastTime) * 1.0e-6f;
  
  // #-1. 改变速度环的设定值
  PID_ChangeSP(&pid_velocity, 0.0f);

  // #-2. 读取传感器的值
  float omega = 0.5 * (App_Encoder_GetSpeed_L() + App_Encoder_GetSpeed_R());
  float theta = App_MPU6050_GetPitch() * 0.0174533; // 单位rad
  float theta_dot = App_MPU6050_GetGx() * 0.0174533; // 单位rad/s

  // #-3. 计算速度环的反馈值x_dot
  float omega2 = -theta_dot * (lp+rw) / rw;
  float omega1 = omega - omega2;
  float x_dot = omega1 * rw;

  // #-4. 执行速度环的PID运算
  float theta_ref = qtan(PID_Compute(&pid_velocity, x_dot) / g);

  // #1. 将外环的设定值SP设置为0
  PID_ChangeSP(&pid_theta, theta_ref);

  // #3. 计算外环PID的输出
  float theta_dot_ref = PID_Compute(&pid_theta, theta);

  // #4. 改变内环的设定值SP
  PID_ChangeSP(&pid_theta_dot, theta_dot_ref);

  // #5. 计算内环PID的输出
  float theta_dot_dot_ref = PID_Compute(&pid_theta_dot, theta_dot);

  // #6. 倒立摆的逆解算
  float x_dot_dot_ref = (g * qsin(theta) - theta_dot_dot_ref * lp) / qcos(theta);

  // #7. 计算轮胎转速
  if(lastTime != 0)
  {
    omega_ref += 1.0f / rw * x_dot_dot_ref * deltaT;
  }
  
  // #8. 设置轮胎转速
  App_Motor_SetOmega_L(omega_ref);
  App_Motor_SetOmega_R(omega_ref);
  
  lastTime = now;

}

//
// @简介：对控制系统进行复位
//
void App_Control_Reset(void)
{
  // #1. 复位暂存的值
  lastTime = 0;
  omega_ref = 0;

  // #2. PID控制器
  PID_Reset(&pid_velocity);
  PID_Reset(&pid_theta);
  PID_Reset(&pid_theta_dot);
}


