#include "stdio.h"
#include "delay.h"
#include "usart.h"
#include "control.h"
#include "track.h"
#include "gear.h"
#include "music.h"
int main(void)
{ 
    delay_init();           //延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    LED_Init();             // LED初始化
    USART1_Init(9600);	    //蓝牙串口初始化为9600
    Motor_Init();           //电机初始化
    Motion_State(OFF);      //关闭电机驱动失能
    Infrared_Init();        //红外传感器初始化
    TIM1_PWM_Init(999,71); //音乐PWM初始化，频率10kHz
    // RGB_LED_Init();//RGB彩灯初始化
    delay_ms(1000);
    while(1)
    {
        Bluetooth_Mode();
    }


}

