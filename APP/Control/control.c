#include "control.h"
#include "motor.h"
#include "gear.h"
#include "music.h"
// ====== 蓝牙、遥控与传感器相关缓冲区 ======
extern char Lx_Buf[10];  // 蓝牙左摇杆数据缓冲区
extern char Rx_Buf[10];  // 蓝牙右摇杆数据缓冲区
extern u8 mode_flag;     // 模式标志位（1=遥控模式，2=重力模式，3=红外循迹）

/**************************************************
映射函数
***************************************************/
int Map(int val,int in_min,int in_max,int out_min,int out_max)
{
	return (int)(val-in_min)*(out_max-out_min)/(in_max-in_min)+out_min;
}

/**************************************************
蓝牙模式
***************************************************/
void Bluetooth_Mode(void)
{
	if(mode_flag==1)
		APP_Joy_Mode(); 	// 软件控制
	else if(mode_flag==2)	
		APP_Gravity_Mode();	// 重力控制
	else if(mode_flag==3)
		Infrared_Mode();	// 红外循迹
	else if(mode_flag==4)
	{
		musicPlay();        // 音乐播放
		mode_flag=1;       // 播放结束后回到遥控模式
	}
}

void Infrared_Mode(void)
{
	uint8_t s7 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7);
    uint8_t s6 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6);
    uint8_t s15 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
    uint8_t s8 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);
	// 1为检测到黑线
    if(s7==1 && s6==1 && s15==1 && s8==1) {
        // 全黑：停车（终点或交叉口）
        Motor_SetLeftSpeed(0);
        Motor_SetRightSpeed(0);
		mode_flag = 1;
    }
    else if(s7==0 && s6==0 && s15==0 && s8==0)
    {
        // 全白：在轨道中间，直行
        Motor_SetLeftSpeed(-50);
        Motor_SetRightSpeed(-50);
    }
	    // ======== 偏左 / 偏右修正 ========
    // 稍偏左（左边检测到黑线）
    else if(s6==1 && s15==0)
    {
        Motor_SetLeftSpeed(-30);
        Motor_SetRightSpeed(-60);
    }
    // 稍偏右（右边检测到黑线）
    else if(s6==0 && s15==1)
    {
        Motor_SetLeftSpeed(-60);
        Motor_SetRightSpeed(-30);
    }
    // 偏左很多（最左检测到黑线）
    else if(s7==1)
    {
        Motor_SetLeftSpeed(-10);
        Motor_SetRightSpeed(-70);
    }
    // 偏右很多（最右检测到黑线）
    else if(s8==1)
    {
        Motor_SetLeftSpeed(-70);
        Motor_SetRightSpeed(-10);
    }

}

void set_motor(uint8_t motor_id, int16_t speed)
{
    uint16_t pwm = (speed >= 0) ? speed : -speed;  // 绝对值作为 PWM 占空比
    if (pwm > 1000) pwm = 1000;  // 限制最大值

    switch (motor_id)
    {
        case 1: // 电机1: PWM → TIM4_CH1(PB6), DIR → PB8
            if (speed >= 0)
			{
                TIM4->CCR1 = speed;   // IN1 → PWM
				TIM4->CCR2 = 0;       // IN2 → 低电平
			}
            else
			{
				TIM4->CCR1 = 0;       // IN1 → 低电平
				TIM4->CCR2 = -speed;   // IN2 → PWM
			}
            break;

        case 2: // 电机2: IN1 → TIM4_CH2(PB7), IN2 → TIM4_CH3(PB8)
			if (speed >= 0)
			{
				TIM4->CCR2 = speed;  // IN1 → PWM
				TIM4->CCR3 = 0;      // IN2 → 低电平
			}
			else
			{
				TIM4->CCR2 = 0;      // IN1 → 低电平
				TIM4->CCR3 = -speed; // IN2 → PWM
			}
			break;

		case 3: // 电机3: IN1 → TIM3_CH1(PA6), IN2 → TIM3_CH3(PB0)
			if (speed >= 0)
			{
				TIM3->CCR1 = speed;  // IN1 → PWM
				TIM3->CCR3 = 0;      // IN2 → 低电平
			}
			else
			{
				TIM3->CCR1 = 0;      // IN1 → 低电平
				TIM3->CCR3 = -speed; // IN2 → PWM
			}
			break;

		case 4: // 电机4: IN1 → TIM3_CH2(PA7), IN2 → TIM3_CH4(PB1)
			if (speed >= 0)
			{
				TIM3->CCR2 = speed;  // IN1 → PWM
				TIM3->CCR4 = 0;      // IN2 → 低电平
			}
			else
			{
				TIM3->CCR2 = 0;      // IN1 → 低电平
				TIM3->CCR4 = -speed; // IN2 → PWM
			}
			break;

        default:
            break;
    }
}

/**************************************************
遥控模式
***************************************************/
void APP_Joy_Mode(void)
{
	int Joy_Lx = 50, Joy_Ly = 50, Joy_Rx = 50;
	int Map_Lx, Map_Ly, Map_Rx;
	int speed_l=0, speed_r=0;
	
	if (Lx_Buf[0] == 'L')
	{
		Joy_Lx = (Lx_Buf[2] - '0') * 10 + (Lx_Buf[3] - '0');
		Joy_Ly = (Lx_Buf[5] - '0') * 10 + (Lx_Buf[6] - '0');
	}
	if (Rx_Buf[0] == 'R')
	{
		Joy_Rx = (Rx_Buf[2] - '0') * 10 + (Rx_Buf[3] - '0');
	}
	Map_Lx = Map(Joy_Lx, 10, 90, -25, 25);
	Map_Ly = Map(Joy_Ly, 10, 90, -100, 100);
	Map_Rx = Map(Joy_Rx, 10, 90, -127, 127); //最大只能127.超过就溢出
	
	if(Map_Ly > 0)
	{
		Map_Lx = -Map_Lx; //Y轴负方向时，X取反，确保倒车顺序正确
	}

	speed_l = Map_Ly - Map_Lx;
	speed_r = Map_Ly + Map_Lx;


	if (speed_l < 15 && speed_l >-15)speed_l = 0;
	if (speed_r < 15 && speed_r >-15)speed_r = 0;	
	if(Map_Rx == 0)
	{
		Motor_SetLeftSpeed(speed_l);
		Motor_SetRightSpeed(speed_r);
	}
	else
	{
		Motor_SetLeftSpeed(-Map_Rx);
		Motor_SetRightSpeed(Map_Rx);
	}
	delay_ms(40);

}

// APP_Gravity_Mode：重力感应控制模式
// 通过手机传过来的 Pitch（俯仰角）、Roll（横滚角）来计算电机 PWM，占空比控制小车运动。
void APP_Gravity_Mode(void)
{
	int i,j=0,Pitch_flag=0;
	int APP_Pitch=0,APP_Roll=0; // Pitch / Roll 数值
	int Pitch_symbel=1,Roll_symbel=1; // Pitch/Roll 符号位（+1 正，-1 负）
	char Pitch_Buf[10],Roll_Buf[10]; // 存放提取出来的 Pitch / Roll 字符串
	int Map_pitch, Map_roll; // 映射后的 Pitch / Roll
	static int Smoothing_Pitch_Buf[5]; // 中值滤波缓存（Pitch）
	static int Smoothing_Roll_Buf[5]; // 中值滤波缓存（Roll）
	static int Smoothing_Count=0; // 中值滤波采样计数
	int Pitch_temp,Roll_temp; // 排序临时变量
	
	// 打开左右电机驱动使能
	L_STBY_ON;
	R_STBY_ON;
	
	// ---------------- 1. 提取 Roll 数据 ----------------
	for(i=1;i<20;i++)
	{
		if(Pitch_Roll_Buf[i]=='.')break; // 遇到小数点停止
		Roll_Buf[i-1]=Pitch_Roll_Buf[i]; // 依次存入 Roll 缓冲区
	}
	// ---------------- 2. 提取 Pitch 数据 ----------------
	for(i=0;i<20;i++)
	{
		if(Pitch_Roll_Buf[i]==',') // 遇到逗号后开始提取 Pitch
		{
			Pitch_flag=1;
			i++;
		}
		if(Pitch_flag==1)
		{
			if(Pitch_Roll_Buf[i]=='.') // 遇到小数点结束
			{
				j=0;
				break;
			}
			Pitch_Buf[j]=Pitch_Roll_Buf[i];
			j++;
		}
	}
	// ---------------- 3. Roll 字符串转数字 ----------------
	j=0;
	for(i=10;i>=0;i--)
	{
		if(Roll_Buf[i]>='0'&&Roll_Buf[i]<='9')
		{
			APP_Roll+=(Roll_Buf[i]-'0')*pow(10,j); // 转换成十进制整数
			j++;
		}
		if(Roll_Buf[0]=='-')
		{
			Roll_symbel=-1;
		}
	}
	// ---------------- 4. Pitch 字符串转数字 ----------------
	j=0;
	for(i=10;i>=0;i--)
	{
		if(Pitch_Buf[i]>='0'&&Pitch_Buf[i]<='9')
		{
			APP_Pitch+=(Pitch_Buf[i]-'0')*pow(10,j);
			j++;
		}
		if(Pitch_Buf[0]=='-')
		{
			Pitch_symbel=-1;
		}
	}
	// 应用符号位得到最终的 Pitch / Roll
	APP_Pitch=Pitch_symbel*APP_Pitch;
	APP_Roll=Roll_symbel*APP_Roll;
	// ---------------- 5. 存入滤波缓冲区 ----------------
	Smoothing_Pitch_Buf[Smoothing_Count]=APP_Pitch;
	Smoothing_Roll_Buf[Smoothing_Count]=APP_Roll;
	Smoothing_Count++;
	// ---------------- 6. 中值滤波处理 ----------------
	if(Smoothing_Count==5)  // 满 5 次后开始处理
	{
		Smoothing_Count=0;
		
		// 对 Pitch / Roll 分别冒泡排序，取中值
		for(j = 0; j < 5 - 1; j++) 
		{
        for(i = 0; i < 5 - j; i++) 
				{
            if(Smoothing_Pitch_Buf[i] > Smoothing_Pitch_Buf[i + 1]) 
						{
                Pitch_temp = Smoothing_Pitch_Buf[i];
                Smoothing_Pitch_Buf[i] = Smoothing_Pitch_Buf[i + 1];
                Smoothing_Pitch_Buf[i + 1] = Pitch_temp;
            }
						if(Smoothing_Roll_Buf[i] > Smoothing_Roll_Buf[i + 1]) 
						{
                Roll_temp = Smoothing_Roll_Buf[i];
                Smoothing_Roll_Buf[i] = Smoothing_Roll_Buf[i + 1];
                Smoothing_Roll_Buf[i + 1] = Roll_temp;
            }			
        }
    }
		// 取排序后的中间值作为最终结果（中值滤波）
		APP_Pitch=Smoothing_Pitch_Buf[2];
		APP_Roll=Smoothing_Roll_Buf[2];
		
		// ---------------- 7. 映射角度 → 控制量 ----------------
		Map_pitch = Map(APP_Pitch, -90, 90, -127, 127);
		Map_roll = Map(APP_Roll, -90, 90, -50, 50);
		
		if(Map_pitch > 0) Map_roll = -Map_roll; // 前进时，Pitch 取反，确保倒车顺序正确
		
		int speed_left  = Map_pitch - Map_roll;
		int speed_right = Map_pitch + Map_roll;
		
		if(abs(speed_left) < 20)speed_left = 0;
		
		if(abs(speed_right) < 20)speed_right = 0;
		
		// 左边两个电机 → PWMA
		if(speed_left >= 0) 
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_4);
			GPIO_ResetBits(GPIOA, GPIO_Pin_5);
			TIM_SetCompare3(TIM2, speed_left);   // PA2 → PWMA
		} else 
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_5);
			GPIO_ResetBits(GPIOA, GPIO_Pin_4);
			TIM_SetCompare3(TIM2, -speed_left);
		}

		// 右边两个电机 → PWMB
		if(speed_right >= 0) 
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_6);
			GPIO_ResetBits(GPIOA, GPIO_Pin_7);
			TIM_SetCompare4(TIM2, speed_right);  // PA3 → PWMB
		} else 
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_7);
			GPIO_ResetBits(GPIOA, GPIO_Pin_6);
			TIM_SetCompare4(TIM2, -speed_right);
		}

		memset(Smoothing_Pitch_Buf,0,sizeof(Smoothing_Pitch_Buf));
		memset(Smoothing_Roll_Buf,0,sizeof(Smoothing_Roll_Buf));
		delay_ms(1);	
	}
	
	memset(Roll_Buf,0,10);
	memset(Pitch_Buf,0,10);
	delay_ms(1);	
}
