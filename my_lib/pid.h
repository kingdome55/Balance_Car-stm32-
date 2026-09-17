#ifndef PID_H
#define PID_H

#include "stm32f10x.h"

typedef struct
{
  float Kp; //比例系数
  float Ki; //积分项系数
  float Kd; //微分项系数
  float SP; //用户的设定值（Setponint）
  
  uint64_t t_k_1;      // t[k-1]，上次运行PID的时间
  float err_k_1;       // err[k-1]，上次运行PID时的误差
  float err_int_k_1;   // err_int[k-1]，上次运行的积分值

  float UpperLimit; //电压CO上限
  float LowerLimit; //电压CO下限
}PID_TypeDef;

void PID_Init(PID_TypeDef *PID, float Kp, float  Ki, float Kd);
void PID_ChangeSP(PID_TypeDef *PID, float SP);
float PID_Compute(PID_TypeDef *PID, float FB);
void PID_LimitConfig(PID_TypeDef *PID, float Upper, float Lower);
void PID_Reset(PID_TypeDef *PID);
#endif
