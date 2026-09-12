#ifndef APP_MPU6050_H
#define APP_MPU6050_H

#include "stm32f10x.h"

void App_MPU6050_Init(void);
void App_MPU6050_Update(void);
void App_MPU6050_Proc(void);

float App_MPU6050_GetAx(void);
float App_MPU6050_GetAy(void);
float App_MPU6050_GetAz(void);

float App_MPU6050_Temperature(void);

float App_MPU6050_GetGx(void);
float App_MPU6050_GetGy(void);
float App_MPU6050_GetGz(void);

float App_MPU6050_GetYaw(void);
float App_MPU6050_GetPitch(void);
float App_MPU6050_GetRoll(void);

#endif
