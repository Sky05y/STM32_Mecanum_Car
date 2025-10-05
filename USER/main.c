/****************************************************************************

麦轮小车引脚分配

N20电机

STBY1:PB4    STBY2:PB9

1:PA2,PB6      2:PA3,PB5
3:PA0,PB8      4:PA1,PB7

蓝牙模块：PA9,PA10


NRF24L01 

CE->PB13`  CSN->PB14  IRQ->PB15  SCK->PA5  MISO->PA6  MOSI->PA7  

|-----------------|  IRQ   MISO 
|       NRF     ::|  MOSI  SCK                       
|      24L01    ::|  CSN   CE
|-----------------|  VCC   GND


超声波模块

TRIG:PB10         ECHO:PB11

RGB彩灯

DI:PB0


************************************************************************************/

#include "stdio.h"
#include "delay.h"
#include "usart.h"
#include "control.h"
#include "track.h"
#include "gear.h"
int main(void)
{ 
    delay_init();           //延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    LED_Init();             // LED初始化
    USART1_Init(9600);	    //蓝牙串口初始化为9600
	// NRF24L01_Init();           //2.4G收发模块
  	// NRF24L01_Check_detection();//NRF24L01等待应答    
    Motor_Init();//电机初始化
    // Hcsr04_Init();//超声波初始化
    Motion_State(OFF);//关闭电机驱动失能
    Infrared_Init();//红外传感器初始化
    SG90_PWM_Init(1999,719);//舵机0初始化
    // RGB_LED_Init();//RGB彩灯初始化
    delay_ms(1000);
    while(1)
    {
        Bluetooth_Mode();
    }


}

