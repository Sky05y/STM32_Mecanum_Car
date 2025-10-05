#ifndef __SG90_PWM_H
#define __SG90_PWM_H

#include "stm32f10x.h"      // 基础类型定义（u16、u32等）
#include "stm32f10x_rcc.h"  // 时钟配置
#include "stm32f10x_gpio.h" // GPIO配置
#include "stm32f10x_tim.h"  // 定时器配置

// ======= 函数声明 =======
void SG90_PWM_Init(u16 arr, u16 psc);
void SG90_Set_Angle(uint8_t angle, uint8_t servo_id);

#endif /* __SG90_PWM_H */
