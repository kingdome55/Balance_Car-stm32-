#ifndef APP_ENCODER_H
#define APP_ENCODER_H

#include "stm32f10x.h"

void App_Encoder_Init(void);
int64_t App_Encoder_GetPos_L(void);
int64_t App_Encoder_GetPos_R(void);

#endif
