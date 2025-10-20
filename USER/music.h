//定义低音音名 （单位：HZ）
#define L1 262
#define L2 294
#define L3 330
#define L4 349
#define L5 392
#define L6 440
#define L7 494

//定义中音音名
#define M1 523
#define M2 587
#define M3 659
#define M4 698
#define M5 784
#define M6 880
#define M7 988

//定义高音音名
#define H1 1047
#define H2 1175
#define H3 1319
#define H4 1397
#define H5 1568
#define H6 1760
#define H7 1976

//全音符所占的时值，单位ms，决定乐谱演奏速度
#define T 2200
#include "delay.h"
#include "stm32f10x.h"
typedef struct
{
	short mName;//音名：取值L1~L7、M1~M7、H1~H7分别表示低音、中音、高音的1234567，取0表示休止符
	short mTime;//时值：取值T、T/2、T/4、T/8、T/16、T/32分别表示全音符、二分音符、四音符、八音符...取0表示演奏结束
	
}tNote;

void TIM1_PWM_Init(u16 arr,u16 psc);
void musicPlay(void);
