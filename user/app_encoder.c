#include "app_encoder.h"

static volatile int64_t encoder_l = 0; // 左电机编码器值
static volatile int64_t encoder_r = 0; // 右电机编码器值
 

