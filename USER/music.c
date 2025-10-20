#include "music.h"

const tNote MyScore[]=
{
  {L6,T/8},{M3,T/8},{M3,T/8},{M3,T/8},{M3,T/4},{M3,T/8},{M2,T/8},
  {M1,T/8},{M1,T/16},{M2,T/16},{M1,T/8},{L7,T/8},{L6,T/4},
  {M6,T/8},{M6,T/8},{M6,T/8},{M6,T/8},{M6,T/4},{M6,T/8},{M5,T/8},
    
  {M3,T/8},{M5,T/8},{M5,T/8},{M4,T/8},{M3,T/2},
  {M3,T/8},{M6,T/8},{M6,T/8},{M5,T/8},{M3,T/4},{M3,T/8},{M2,T/8},
  {M1,T/8},{M2,T/8},{M1,T/8},{L7,T/8},{L6,T/4},{L3,T/4},
  
  {L3,T/8},{M1,T/8},{M1,T/8},{L7,T/8},{L6,T/4},{L6,T/8},{M3,T/8},
  {M2,T/8},{M2,T/16},{M1,T/16},{L7,T/8},{L5,T/8},{L6,T/2},
  ///////////////////////////////////////////////////////////////
  {L6,T/8},{M3,T/8},{M3,T/8},{M3,T/8},{M3,T/4},{M3,T/8},{M2,T/8},
  {M1,T/8},{M1,T/16},{M2,T/16},{M1,T/8},{L7,T/8},{L6,T/4},
  {M6,T/8},{M6,T/8},{M6,T/8},{M6,T/8},{M6,T/4},{M6,T/8},{M5,T/8},
    
  {M3,T/8},{M5,T/8},{M5,T/8},{M4,T/8},{M3,T/2},
  {M3,T/8},{M6,T/8},{M6,T/8},{M5,T/8},{M3,T/4},{M3,T/8},{M2,T/8},
  {M1,T/8},{M2,T/8},{M1,T/8},{L7,T/8},{L6,T/4},{L3,T/4},
  
  {L3,T/8},{M1,T/8},{M1,T/8},{L7,T/8},{L6,T/4},{L6,T/8},{M3,T/8},
  {M2,T/8},{M2,T/16},{M1,T/16},{L7,T/8},{L5,T/8},{L6,T/2},

  {0,0}//结束
};

/**************************************************
函数名：TIM2_PWM_Init
功能描述：使用 PA1 (TIM2_CH2) 输出 PWM 波形
参数：
    arr —— 自动重装载值（决定PWM周期）
    psc —— 预分频系数（决定PWM频率）
***************************************************/
void TIM1_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  // TIM1时钟	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // GPIOA时钟
   //设置该引脚为复用输出功能,输出TIM1 CH1 PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
 
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM1PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设
 
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE 主输出使能		
 
	TIM_Cmd(TIM1, ENABLE);  //使能TIM1

}
//蜂鸣器停止发声
void buzzerQuiet(void)
{
	TIM_Cmd(TIM1,DISABLE);//不使能TIM1
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);//PA8输出低
}

//蜂鸣器发出指定频率声音
void buzzerSound(unsigned short usFrep)
{
  unsigned long ulVal;
	if((usFrep<=1000000/65536UL)||(usFrep>20000))//1000000即100k，也就是上面所说
												//65535是计数器最大数值
	{
	  buzzerQuiet();//静音
	}
   else
	 {
		 ulVal=1000000/usFrep;
		 TIM1->ARR=ulVal;//设置自动重装载寄存器周期的值（音调）
		 TIM_SetCompare1(TIM1,ulVal/5);//设置比较值，调节占空比（音量）
		 TIM_Cmd(TIM1,ENABLE);//使能TIM1
	 }
}


//演奏乐曲
void musicPlay(void)
{
	u8 i=0;
	while(1)
	{
	  if(MyScore[i].mTime==0)break;
		buzzerSound(MyScore[i].mName);
		delay_ms(MyScore[i].mTime);
		i++;
		buzzerQuiet();
		delay_ms(10);
	}

}



