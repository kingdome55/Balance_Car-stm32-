#include "qmath.h"

// 0~90度，1000个采样点的sin值表
static const float sin_val[1000] = {
    #include "sin_table.inc"
};

float qsin(float x)
{
    // 1. 角度对360取模，限制到0~360°范围
    x = fmodf(x, 360.0f);
    if(x < 0) x += 360.0f;
    
    float res;
    int idx;
    if(x >= 0 && x <= 90.0f)
    {
        // 0~90°，直接查表
        idx = (int)(x / 90.0f * 1000.0f);
        if(idx >= 1000) idx = 999;
        res = sin_val[idx];
    }
    else if(x > 90.0f && x <= 180.0f)
    {
        // sin(180-α)=sinα
        idx = (int)((180.0f - x) / 90.0f * 1000.0f);
        if(idx >= 1000) idx = 999;
        res = sin_val[idx];
    }
    else if(x > 180.0f && x <= 270.0f)
    {
        // sin(180+α)= -sinα
        idx = (int)((x - 180.0f) / 90.0f * 1000.0f);
        if(idx >= 1000) idx = 999;
        res = -sin_val[idx];
    }
    else
    {
        // sin(360-α)= -sinα
        idx = (int)((360.0f - x) / 90.0f * 1000.0f);
        if(idx >= 1000) idx = 999;
        res = -sin_val[idx];
    }
    return res;
}

float qcos(float x)
{
    // 图中公式：cos(x)=sin(x+90°)
    return qsin(x + 90.0f);
}

float qtan(float x)
{
    // 图中公式 tan(x)=sin(x)/cos(x)
    float s = qsin(x);
    float c = qcos(x);
    if(c == 0)
    {
        return 1e6f; // 无穷大近似
    }
    return s / c;
}

// 下面反三角函数：输入是比值（-1~1），返回值是角度（度），与 qsin/qcos 的单位约定一致
float qasin(float x)
{
    return asinf(x) * 180.0f / 3.1415926f;
}
float qacos(float x)
{
    return acosf(x) * 180.0f / 3.1415926f;
}
float qatan(float x)
{
    return atanf(x) * 180.0f / 3.1415926f;
}
float qatan2(float y, float x)
{
    return atan2f(y, x) * 180.0f / 3.1415926f;
}
