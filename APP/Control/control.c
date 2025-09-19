#include "control.h"
#include "motor.h"
u8 Joy_RxBuf[20];//ҡ�˽������ݻ�����
u8 MPU_RxBuf[10];//�����ǽ������ݻ�����
u8 Joy_Lpos,Joy_Rpos;//ҡ����������
u8 MPU_Data[10];//MPU���������˲�����
u8 Con_RxBuf[10];//����ָ���������
u8 Eva_RxBuf[4];//����ָֹͣ������
u8 RGB_RxBuf[4];//�ʵ�ָֹͣ������
u8 mode=1;//ģʽ��1Ĭ��Ϊ����ģʽ
extern u8 mode_flag;
u8 NRF_flag=0;//ң��ģʽ�ж�ѭ���˳���־λ
u8 MPU_flag=0;//����ģʽ�ж�ѭ���˳���־λ

u8 RGB_flag=0;//RGB�ʵƱ�־λ
u8 RGB_mode=0;//RGB�ʵ�ģʽ
int HSV_H=0;//HSV��H��ֵ
u8 HSV_flag=0;//��ɫת��ʱ���ñ�־λ
u8 LED_Count=0;//LED�Ƶĸ���


extern char Lx_Buf[10];//���ڽ���ҡ�����ݻ�����
extern char Rx_Buf[10];//���ڽ���ҡ�����ݻ�����



/**************************************************
�������ƣ�Map(int val,int in_min,int in_max,int out_min,int out_max)
�������ܣ�ӳ�亯��
��ڲ�����val:��Ҫӳ�����ֵ
					in_min:����ֵ�ķ�Χ��Сֵ
					in_max:����ֵ�ķ�Χ���ֵ
					out_min:���ֵ�ķ�Χ��Сֵ
					out_max:���ֵ�ķ�Χ���ֵ

���ز�����ӳ������ֵ
***************************************************/
int Map(int val,int in_min,int in_max,int out_min,int out_max)
{
	return (int)(val-in_min)*(out_max-out_min)/(in_max-in_min)+out_min;
}


/**************************************************
�������ƣ�Bluetooth_Mode(void)
�������ܣ�����ң��ģʽ
��ڲ�������
���ز�������
***************************************************/
void Bluetooth_Mode(void)
{
	// if(mode_flag==1)
	// {
	// 	APP_Joy_Mode();//APPҡ��ģʽ
	// }
	// else if(mode_flag==2)APP_Gravity_Mode();//APP����ģʽ
	// else if(mode_flag==3)Evadible_Mode();
	// else if(mode_flag==4)Follow_Mode();
	// else if(mode_flag==5)RGB_mode=0;//������
	// else if(mode_flag==6)RGB_mode=1;//��ˮ��
	// else if(mode_flag==7)RGB_mode=2;//��˸��
	// else Motion_State(ON);
	if(mode_flag==1)
		APP_Joy_Mode();
	else if(mode_flag==2)
		APP_Gravity_Mode();
}

/**************************************************
�������ƣ�Wireless_Mode(void)
�������ܣ�ң��ģʽ
��ڲ�������
���ز�������
***************************************************/
void Wireless_Mode(void)
{
	int Joy_Lx=50, Joy_Ly = 50, Joy_Rx = 50, Joy_Ry = 50;
	int Map_Lx, Map_Ly, Map_Rx, Map_Ry;
	int pwm1, pwm2, pwm3, pwm4;
	static u8 RGB_mode1=0,i=0;
	static u8 RGB_mode2=0,j=0;
	NRF24L01_RX_Mode();
	L_STBY_ON;
	R_STBY_ON;
	if(NRF24L01_RxPacket(Joy_RxBuf)==0)
	{
		  Joy_Lx = (Joy_RxBuf[2] - '0') * 10 + (Joy_RxBuf[3] - '0');
			Joy_Ly = (Joy_RxBuf[5] - '0') * 10 + (Joy_RxBuf[6] - '0');
			
			Joy_Rx = (Joy_RxBuf[9] - '0') * 10 + (Joy_RxBuf[10] - '0');
			Joy_Ry = (Joy_RxBuf[12] - '0') * 10 + (Joy_RxBuf[13] - '0');

			Map_Lx = Map(Joy_Lx, 10, 90, -127, 127);
			Map_Ly = Map(Joy_Ly, 10, 90, -127, 127);
			Map_Rx = Map(Joy_Rx, 10, 90, -127, 127);
			Map_Ry = Map(Joy_Ry, 10, 90, -127, 127);

			
			pwm1 = -Map_Ly - Map_Lx - Map_Ry - Map_Rx;
			pwm2 = -Map_Ly + Map_Lx - Map_Ry + Map_Rx;
			pwm3 = -Map_Ly - Map_Lx - Map_Ry + Map_Rx;
			pwm4 = -Map_Ly + Map_Lx - Map_Ry - Map_Rx;


			pwm1 = Map(pwm1, -127, 127, -499, 499);
			pwm2 = Map(pwm2, -127, 127, -499, 499);
			pwm3 = Map(pwm3, -127, 127, -499, 499);
			pwm4 = Map(pwm4, -127, 127, -499, 499);


			if (pwm1 < 20 && pwm1 >-20)pwm1 = 0;
			if (pwm2 < 20 && pwm2 >-20)pwm2 = 0;
			if (pwm3 < 20 && pwm3 >-20)pwm3 = 0;
			if (pwm4 < 20 && pwm4 >-20)pwm4 = 0;

			if (pwm1 > 499)pwm1 = 499;
			if (pwm2 > 499)pwm2 = 499;
			if (pwm3 > 499)pwm3 = 499;
			if (pwm4 > 499)pwm4 = 499;
			
			if (pwm1 < -499)pwm1 = -499;
			if (pwm2 < -499)pwm2 = -499;
			if (pwm3 < -499)pwm3 = -499;
			if (pwm4 < -499)pwm4 = -499;
			
			if(pwm1>=0)
			{
				TIM_SetCompare4(TIM2,500-pwm1);//L_BIN2:������
				L_BIN2_ON;
				
			}
			else if(pwm1<0)
			{
				pwm1=abs(pwm1);
				TIM_SetCompare4(TIM2,pwm1);//L_BIN2:������
				L_BIN2_OFF;	
			}
			
			if(pwm2>=0)
			{
				TIM_SetCompare3(TIM2,pwm2);//L_AIN2:������
				L_AIN2_OFF;
			}
			else if(pwm2<0)
			{
				pwm2=abs(pwm2);
				TIM_SetCompare3(TIM2,500-pwm2);//L_AIN2:������
				L_AIN2_ON;
			}
					
			if(pwm3>=0)
			{
				TIM_SetCompare1(TIM2,500-pwm3);//R_AIN2:������
				R_AIN2_ON;
			}
			else if(pwm3<0)
			{
				pwm3=abs(pwm3);
				TIM_SetCompare1(TIM2,pwm3);//R_AIN2:������
				R_AIN2_OFF;
			}
					
			if(pwm4>=0)
			{
				TIM_SetCompare2(TIM2,pwm4);//R_BIN2:������
				R_BIN2_OFF;
			}
			else if(pwm4<0)
			{
				pwm4=abs(pwm4);
				TIM_SetCompare2(TIM2,500-pwm4);//R_BIN2:������
				R_BIN2_ON;
			}
			
			if(Joy_RxBuf[0]=='L'&&Joy_RxBuf[3]=='R')
			{
				Motion_State(ON);
				NRF_flag=1;
			}
		printf((char*)Joy_RxBuf); 
		printf("\n"); 
	}
	else
	{	
 		printf("Receive failed !\n"); 
	}
	
	if(RGB_mode==0)
	{
		//RGB�ʵƺ�����ˮ��Ч��
		hsv_to_rgb(HSV_H,100,100,&RGB_R,&RGB_G,&RGB_B);
		if(HSV_H==360)HSV_flag=1;
		else if(HSV_H==0)HSV_flag=0; 
		if(HSV_flag==0)HSV_H+=2;
		else if(HSV_flag==1)HSV_H-=2;
		for(LED_Count=0;LED_Count<6;LED_Count++)
		{
			RGB_LED_Write_24Bits(RGB_R, RGB_G, RGB_B);
		}
	}
	else if(RGB_mode==1)
	{	
		if(i%20==0)
		{
			for(LED_Count=0;LED_Count<6;LED_Count++)
			{
				hsv_to_rgb((RGB_mode1%360),100,100,&RGB_R,&RGB_G,&RGB_B);
				RGB_LED_Write_24Bits(RGB_R, RGB_G, RGB_B);
				RGB_mode1+=60;
				if(RGB_mode1>360)
				{
					RGB_mode1=0;
					i=0;
				}
			}
		}
		i++;
	}
	else if(RGB_mode==2)	
	{
		RGB_Circulation(RGB_mode2);
		if(j%20==0)RGB_mode2++;
		if(RGB_mode2>8)
		{
			j=0;
			RGB_mode2=0;
		}
		j++;
	}
	
	delay_ms(10);	
}

/**************************************************
�������ƣ�Evadible_Mode(void)
�������ܣ�����ģʽ
��ڲ�������
���ز�������
***************************************************/
void Evadible_Mode(void)
{
	float Dis;
	Dis=Hcsr04GetLength();//������ģ���ȡ����
	forward(200);//ǰ��

	if(Dis<=15)
	{
		backward(200);//����
		delay_ms(400);
		Left_Turn(200);//��ת
		delay_ms(400);
	}
}

/**************************************************
�������ƣ�Gravity_Mode(void)
�������ܣ�����ģʽ
��ڲ�������
���ز�������
***************************************************/
void Gravity_Mode(void)
{
	int MPU_pitch=50, MPU_roll = 50;
	int Map_pitch, Map_roll;
	int pwm1, pwm2, pwm3, pwm4;
	static u8 RGB_mode1=0,i=0;
	static u8 RGB_mode2=0,j=0;
	NRF24L01_RX_Mode();
	L_STBY_ON;
	R_STBY_ON;
	if(NRF24L01_RxPacket(MPU_RxBuf)==0)
	{
		  MPU_pitch = (MPU_RxBuf[1] - '0') * 10 + (MPU_RxBuf[2] - '0');
			MPU_roll = (MPU_RxBuf[4] - '0') * 10 + (MPU_RxBuf[5] - '0');
						
			Map_pitch = Map(MPU_pitch, 10, 90, -127, 127);
			Map_roll = Map(MPU_roll, 10, 90, -127, 127);
				
			pwm1 =  Map_pitch + Map_roll;
			pwm2 =  Map_pitch - Map_roll;
			pwm3 =  Map_pitch + Map_roll;
			pwm4 =  Map_pitch - Map_roll;


			pwm1 = Map(pwm1, -127, 127, -499, 499);
			pwm2 = Map(pwm2, -127, 127, -499, 499);
			pwm3 = Map(pwm3, -127, 127, -499, 499);
			pwm4 = Map(pwm4, -127, 127, -499, 499);


			if (pwm1 < 20 && pwm1 >-20)pwm1 = 0;
			if (pwm2 < 20 && pwm2 >-20)pwm2 = 0;
			if (pwm3 < 20 && pwm3 >-20)pwm3 = 0;
			if (pwm4 < 20 && pwm4 >-20)pwm4 = 0;

			if (pwm1 > 499)pwm1 = 499;
			if (pwm2 > 499)pwm2 = 499;
			if (pwm3 > 499)pwm3 = 499;
			if (pwm4 > 499)pwm4 = 499;
			
			if (pwm1 < -499)pwm1 = -499;
			if (pwm2 < -499)pwm2 = -499;
			if (pwm3 < -499)pwm3 = -499;
			if (pwm4 < -499)pwm4 = -499;
			
			if(pwm1>=0)
			{
				TIM_SetCompare4(TIM2,500-pwm1);//L_BIN2:������
				L_BIN2_ON;
				
			}
			else if(pwm1<0)
			{
				pwm1=abs(pwm1);
				TIM_SetCompare4(TIM2,pwm1);//L_BIN2:������
				L_BIN2_OFF;	
			}
			
			if(pwm2>=0)
			{
				TIM_SetCompare3(TIM2,pwm2);//L_AIN2:������
				L_AIN2_OFF;
			}
			else if(pwm2<0)
			{
				pwm2=abs(pwm2);
				TIM_SetCompare3(TIM2,500-pwm2);//L_AIN2:������
				L_AIN2_ON;
			}
					
			if(pwm3>=0)
			{
				TIM_SetCompare1(TIM2,500-pwm3);//R_AIN2:������
				R_AIN2_ON;
			}
			else if(pwm3<0)
			{
				pwm3=abs(pwm3);
				TIM_SetCompare1(TIM2,pwm3);//R_AIN2:������
				R_AIN2_OFF;
			}
					
			if(pwm4>=0)
			{
				TIM_SetCompare2(TIM2,pwm4);//R_BIN2:������
				R_BIN2_OFF;
			}
			else if(pwm4<0)
			{
				pwm4=abs(pwm4);
				TIM_SetCompare2(TIM2,500-pwm4);//R_BIN2:������
				R_BIN2_ON;
			}
					
			
			if(MPU_RxBuf[0]=='M'&&MPU_RxBuf[1]=='P'&&MPU_RxBuf[2]=='U')
			{
				Motion_State(ON);
				MPU_flag=1;
			}
	}
	else
	{	
 		printf("Receive failed !\n"); 
	}
	
	if(RGB_mode==0)
	{
		//RGB�ʵƺ�����ˮ��Ч��
		hsv_to_rgb(HSV_H,100,100,&RGB_R,&RGB_G,&RGB_B);
		if(HSV_H==360)HSV_flag=1;
		else if(HSV_H==0)HSV_flag=0; 
		if(HSV_flag==0)HSV_H+=2;
		else if(HSV_flag==1)HSV_H-=2;
		for(LED_Count=0;LED_Count<6;LED_Count++)
		{
			RGB_LED_Write_24Bits(RGB_R, RGB_G, RGB_B);
		}
	}
	else if(RGB_mode==1)
	{	
		if(i%20==0)
		{
			for(LED_Count=0;LED_Count<6;LED_Count++)
			{
				hsv_to_rgb((RGB_mode1%360),100,100,&RGB_R,&RGB_G,&RGB_B);
				RGB_LED_Write_24Bits(RGB_R, RGB_G, RGB_B);
				RGB_mode1+=60;
				if(RGB_mode1>360)
				{
					RGB_mode1=0;
					i=0;
				}
			}
		}
		i++;
	}
	else if(RGB_mode==2)	
	{
		RGB_Circulation(RGB_mode2);
		if(j%20==0)RGB_mode2++;
		if(RGB_mode2>8)
		{
			j=0;
			RGB_mode2=0;
		}
		j++;
	}
	
	delay_ms(10);	
}

/**************************************************
�������ƣ�Follow_Mode(void)
�������ܣ�����ģʽ
��ڲ�������
���ز�������
***************************************************/
void Follow_Mode(void)
{
	float Dis;
	Dis=Hcsr04GetLength();//������ģ���ȡ����
	if(Dis<=10)
	{
		backward(200);//����
	}
	else if(Dis<=30&&Dis>=20)
	{
		forward(200);//ǰ��
	}
	else Motion_State(ON);
}

/**************************************************
�������ƣ�void RGB_Select(void)
�������ܣ��ʵ�ģʽ������ѡ��
��ڲ�������
���ز�������
***************************************************/
void RGB_Select(void)
{
	Motion_State(ON);
	NRF24L01_RX_Mode();
	if(NRF24L01_RxPacket(RGB_RxBuf)==0&&RGB_RxBuf[0]!=' ')
	{
		if(RGB_RxBuf[0]=='S'&&RGB_RxBuf[1]=='T'&&RGB_RxBuf[2]=='O'&&RGB_RxBuf[3]=='P')
		{
			RGB_flag=1;
		}
		if(RGB_RxBuf[2]=='0')RGB_mode=0;//������
		else if(RGB_RxBuf[2]=='1')RGB_mode=1;//��ˮ��
		else if(RGB_RxBuf[2]=='2')RGB_mode=2;//��˸��
		
	}
	delay_ms(10);
	printf("RGB_mode=%d\n",RGB_mode);
}

/**************************************************
�������ƣ�RGB_Show(void)
�������ܣ��ʵ���ʾ
��ڲ�������
���ز�������
***************************************************/
void RGB_Show(void)
{
	static u8 RGB_mode1=0,i=0;
	static u8 RGB_mode2=0,j=0;
	if(RGB_mode==0)
	{
			//RGB�ʵƺ�����ˮ��Ч��
		hsv_to_rgb(HSV_H,100,100,&RGB_R,&RGB_G,&RGB_B);
		if(HSV_H==360)HSV_flag=1;
		else if(HSV_H==0)HSV_flag=0; 
		if(HSV_flag==0)HSV_H+=10;
		else if(HSV_flag==1)HSV_H-=10;
		for(LED_Count=0;LED_Count<6;LED_Count++)
		{
			RGB_LED_Write_24Bits(RGB_R, RGB_G, RGB_B);
		}
		delay_ms(100);
	}
	else if(RGB_mode==1)
	{	
		if(i%3==0)
		{
			for(LED_Count=0;LED_Count<6;LED_Count++)
			{
				hsv_to_rgb((RGB_mode1%360),100,100,&RGB_R,&RGB_G,&RGB_B);
				RGB_LED_Write_24Bits(RGB_R, RGB_G, RGB_B);
				RGB_mode1+=60;
				if(RGB_mode1>360)
				{
					RGB_mode1=0;
					i=0;
				}
			}
		}
		i++;
		delay_ms(100);
	}
	else if(RGB_mode==2)	
	{
		RGB_Circulation(RGB_mode2);
		if(j%3==0)RGB_mode2++;
		if(RGB_mode2>8)
		{
			j=0;
			RGB_mode2=0;
		}
		j++;
		delay_ms(100);
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
�������ƣ�Joy_Mode(void)
�������ܣ�����ģʽ��ҡ��ң��
��ڲ�������
���ز�������
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

	speed_l = Map_Ly + Map_Lx;
	speed_r = Map_Ly - Map_Lx;


	if (speed_l < 15 && speed_l >-15)speed_l = 0;
	if (speed_r < 15 && speed_r >-15)speed_r = 0;	
	if(Map_Rx == 0)
	{
		Motor_SetLeftSpeed(speed_l);
		Motor_SetRightSpeed(speed_r);
	}
	else
	{
		Motor_SetLeftSpeed(Map_Rx);
		Motor_SetRightSpeed(-Map_Rx);
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
	int pwm1, pwm2, pwm3, pwm4; // 四个电机的 PWM 值
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
		Map_roll = Map(APP_Roll, -90, 90, -127, 127);
					
		pwm1 =  -Map_pitch + Map_roll;
		pwm2 =  -Map_pitch - Map_roll;
		pwm3 =  -Map_pitch + Map_roll;
		pwm4 =  -Map_pitch - Map_roll;
		
		
		pwm1 = Map(pwm1, -127, 127, -499, 499);
		pwm2 = Map(pwm2, -127, 127, -499, 499);
		pwm3 = Map(pwm3, -127, 127, -499, 499);
		pwm4 = Map(pwm4, -127, 127, -499, 499);

		
		
		if (pwm1 < 20 && pwm1 >-20)pwm1 = 0;
		if (pwm2 < 20 && pwm2 >-20)pwm2 = 0;
		if (pwm3 < 20 && pwm3 >-20)pwm3 = 0;
		if (pwm4 < 20 && pwm4 >-20)pwm4 = 0;

		if (pwm1 > 499)pwm1 = 499;
		if (pwm2 > 499)pwm2 = 499;
		if (pwm3 > 499)pwm3 = 499;
		if (pwm4 > 499)pwm4 = 499;
				
		if (pwm1 < -499)pwm1 = -499;
		if (pwm2 < -499)pwm2 = -499;
		if (pwm3 < -499)pwm3 = -499;
		if (pwm4 < -499)pwm4 = -499;
				

		
		if(pwm1>=0)
		{
			TIM_SetCompare4(TIM2,500-pwm1);//L_BIN2:������
			L_BIN2_ON;
			
		}
		else if(pwm1<0)
		{
			pwm1=abs(pwm1);
			TIM_SetCompare4(TIM2,pwm1);//L_BIN2:������
			L_BIN2_OFF;	
		}
				
		if(pwm2>=0)
		{
			TIM_SetCompare3(TIM2,pwm2);//L_AIN2:������
			L_AIN2_OFF;
		}
		else if(pwm2<0)
		{
			pwm2=abs(pwm2);
			TIM_SetCompare3(TIM2,500-pwm2);//L_AIN2:������
			L_AIN2_ON;
		}
					
		if(pwm3>=0)
		{
			TIM_SetCompare1(TIM2,500-pwm3);//R_AIN2:������
			R_AIN2_ON;
		}
		else if(pwm3<0)
		{
			pwm3=abs(pwm3);
			TIM_SetCompare1(TIM2,pwm3);//R_AIN2:������
			R_AIN2_OFF;
		}
					
		if(pwm4>=0)
		{
			TIM_SetCompare2(TIM2,pwm4);//R_BIN2:������
			R_BIN2_OFF;
		}
		else if(pwm4<0)
		{
			pwm4=abs(pwm4);
			TIM_SetCompare2(TIM2,500-pwm4);//R_BIN2:������
			R_BIN2_ON;
		}	

		memset(Smoothing_Pitch_Buf,0,sizeof(Smoothing_Pitch_Buf));
		memset(Smoothing_Roll_Buf,0,sizeof(Smoothing_Roll_Buf));
		delay_ms(1);	
	}
	
	memset(Roll_Buf,0,10);
	memset(Pitch_Buf,0,10);
	delay_ms(1);	
}

/**************************************************
�������ƣ�void Control(void)
�������ܣ�����С�����ƺ���
��ڲ�������
���ز�������
***************************************************/
void Control(void)
{
	
	NRF24L01_RX_Mode();
	if(NRF24L01_RxPacket(Con_RxBuf)==0&&Con_RxBuf[0]!=' ')
	{
		if(Con_RxBuf[0]=='L'&&Con_RxBuf[1]=='Y')//����ģʽ
		{
			mode=1;
		}
		else if(Con_RxBuf[0]=='L'&&Con_RxBuf[1]=='X')//ң��ģʽ
		{
			mode=2;
		}
		else if(Con_RxBuf[0]>='P'&&Con_RxBuf[3]<='R')//������Ӧģʽ
		{
			mode=3;
		}
		else if(Con_RxBuf[0]=='B'&&Con_RxBuf[1]=='Z')//����ģʽ
		{
			mode=4;
		}
		else if(Con_RxBuf[0]=='G'&&Con_RxBuf[1]=='S')//����ģʽ
		{
			mode=5;
		}
		else if(Con_RxBuf[0]=='C'&&Con_RxBuf[1]=='D')//RGB�ʵ�ģʽ
		{
			mode=6;
		}
		delay_ms(10);
		//printf("mode=%d\n",mode);
	}
	else
	{
		mode=1;
	}
	delay_ms(10);

	if(mode==1)
	{
		Bluetooth_Mode();
	}
	else if(mode==2)
	{
		while(1)
		{
			Wireless_Mode();//ң��ģʽ
			if(NRF_flag==1)
			{
				mode=1;
				break;
			}
		}
	}
	else if(mode==3)
	{
		while(1)
		{
			Gravity_Mode();//����ģʽ
			if(MPU_flag==1)
			{
				mode=1;
				break;
			}
		}
	}
	else if(mode==4)
	{
		Evadible_Mode();//����ģʽ
	}
	else if(mode==5)
	{
		Follow_Mode();//����ģʽ
	}
	else if(mode==6)
	{
		while(1)
		{
			RGB_Select();//RGB�ʵ�ѡ��
			if(RGB_flag==1)
			{
				mode=1;
				break;
			}
		}
	}
	
	MPU_flag=0;
	NRF_flag=0;
	RGB_flag=0;
	
	RGB_Show();//RGB�ʵ���ʾ
	
}

