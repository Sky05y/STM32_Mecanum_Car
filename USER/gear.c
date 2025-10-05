#include "stm32f10x.h"                  // STM32 标准外设库核心头文件
#include "stm32f10x_rcc.h"              // RCC 时钟配置
#include "stm32f10x_gpio.h"             // GPIO 配置
#include "stm32f10x_tim.h"              // 定时器配置

void SG90_PWM_Init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // 使能 TIM3和GPIOB 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 配置 PB0,PB1 为复用推挽输出（TIM3_CH3）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置 TIM3 基本定时器
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 配置 PWM 输出模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    // PB0
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // PB1
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // 使能自动重装载寄存器
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    // 启动定时器
    TIM_Cmd(TIM3, ENABLE);
}

void SG90_Set_Angle(uint8_t angle, uint8_t servo_id)
{
    if (angle > 180)
        angle = 180;
    uint16_t duty = 10 * (angle + 45) / 9; // 角度转化为占空比
 
    switch (servo_id)
    {
        case 1: TIM_SetCompare3(TIM3, duty); break;  // PB0 -> 舵机1
        case 2: TIM_SetCompare4(TIM3, duty); break;  // PB1 -> 舵机2
        default: break; // 未来可以扩展其他通道
    }
}
