#ifndef __PWM_H
#define __PWM_H	
#include "sys.h" 

void PWM1_Init(u32 arr,u32 psc);
void PWM2_Init(u32 arr,u32 psc);
void PWM3_Init(u32 arr,u32 psc);
void PWM4_Init(u32 arr,u32 psc);
void PWM_Init(void);
void PWM_SetCompare3(uint16_t Compare);
void PWM_Text(void);
int PWM(int angle);

	
#endif


