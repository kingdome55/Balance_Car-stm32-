// 生成0~90度，1000个点 sin(角度转弧度)
// 示例，你可以运行这个脚本生成完整数组：
/*
#include <stdio.h>
#include <math.h>
#define PI 3.1415926535f
int main(void)
{
    for(int i=0;i<1000;i++)
    {
        float deg = i * 90.0f / 1000.0f;
        float s = sinf(deg * PI / 180.0f);
        printf("%.6ff,",s);
        if(i%10 == 0) printf("\n");
    }
    return 0;
}
*/
