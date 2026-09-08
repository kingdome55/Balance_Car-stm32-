#ifndef APP_ENCODER_H
#define APP_ENCODER_H

#include "stm32f10x.h"

void App_Encoder_Init(void);
float App_Encoder_GetPos_L(void);
float App_Encoder_GetPos_R(void);
float App_Encoder_GetSpeed_L(void);
float App_Encoder_GetSpeed_R(void);

#endif
