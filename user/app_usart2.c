#include "app_usart2.h"
//
// 简介： 对串口2初始化
//

void App_USART2_Init(void)
{
    // #1. 对PA2和PA3进行初始化，PA2 - Tx - AF_PP，PA3 - Rx - IPU
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 开启GPIOA的时钟

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // PA2 - Tx - AF_PP
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz; // 921600

    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PA3 - Rx - IPU
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;

    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // #2. 开启USART2的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // #3. 初始化USART2的参数
    USART_InitTypeDef USART_InitStruct = {0};
    USART_InitStruct.USART_BaudRate = 921600;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;

    USART_Init(USART2, &USART_InitStruct);

    // #4. 闭合USART2的总开关
    USART_Cmd(USART2, ENABLE);

}
