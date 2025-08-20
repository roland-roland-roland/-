#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "NRF24L01.h"
#include "LED.h"
#include "MPU6050.h"	
#include "OLED.h"
#include "Motor.h"
#include "CalculateAngles.h"

// 全局变量定义
uint16_t Byte[6];
uint8_t MPU6050Data[12];
uint8_t Stutes;
int16_t Target_Pitch_Angle, Target_Roll_Angle, Now_Pitch_Angle, Now_Roll_Angle;
int16_t Error_Pitch_Angle, Error_Roll_Angle;
int16_t Last_Error_Pitch, Last_Error_Roll;  // 上一次误差
int32_t Integral_Pitch, Integral_Roll;      // 积分项
int16_t Derivative_Pitch, Derivative_Roll;  // 微分项

// PID参数
float Kp = 0.3f;    // 比例系数
float Ki = 0.0f;    // 积分系数
float Kd = 0.02f;   // 微分系数

// 限制参数
#define INTEGRAL_LIMIT 500    // 积分限幅
#define PID_OUTPUT_LIMIT 100  // PID输出限幅

int8_t BasicSpeed;
int16_t PID_Output_Pitch, PID_Output_Roll;
int16_t FR_MotorSpeed, FL_MotorSpeed, BR_MotorSpeed, BL_MotorSpeed;
int16_t BR_BasicSpeed,FR_BasicSpeed,FL_BasicSpeed,BL_BasicSpeed;



int main(void)
{	
    Delay_ms(500);	// 等待上电稳定
	
	/******************************
	初始化 	
	******************************/
	LED_Init();
	NRF24L01_Init();
	MPU6050_Init();
	OLED_Init();
	Motor_Init();
	
	LED_Flicker();
	
	// 初始化目标角度和基础速度
	Target_Pitch_Angle = 0;
	Target_Roll_Angle = 0;
	BasicSpeed = 20;
	
	// 初始化PID相关变量
	Last_Error_Pitch = 0;
	Last_Error_Roll = 0;
	Integral_Pitch = 0;
	Integral_Roll = 0;
	
	BR_BasicSpeed = 1;
	FR_BasicSpeed = 0;
	FL_BasicSpeed = 0;
	BL_BasicSpeed = 0;
	
	OLED_ShowString(2,1,"FL");
	OLED_ShowString(2,6,"FR");
	OLED_ShowString(3,1,"BL");
	OLED_ShowString(3,6,"BR");
	
    while(1)
    {		
		if(ErrorStutes != 1)  // 无错误状态
		{
			// 接收NRF24L01数据
			if(NRF24L01_Receive() == 1)	
			{
				BasicSpeed = NRF24L01_Rx_Packet[0] * 0.9;
				Stutes = NRF24L01_Rx_Packet[1];
			}
			
			// 计算角度
			CalculateAngles();
   
			Now_Pitch_Angle = pitch;
			Now_Roll_Angle = roll;
			
			// 计算当前误差
			Error_Pitch_Angle = Now_Pitch_Angle - Target_Pitch_Angle;
			Error_Roll_Angle = Now_Roll_Angle - Target_Roll_Angle;
			
			// 计算积分项 (带限幅)
			Integral_Pitch += Error_Pitch_Angle;
			Integral_Roll += Error_Roll_Angle;
			
			// 积分限幅，防止积分饱和
			if(Integral_Pitch > INTEGRAL_LIMIT) Integral_Pitch = INTEGRAL_LIMIT;
			else if(Integral_Pitch < -INTEGRAL_LIMIT) Integral_Pitch = -INTEGRAL_LIMIT;
			
			if(Integral_Roll > INTEGRAL_LIMIT) Integral_Roll = INTEGRAL_LIMIT;
			else if(Integral_Roll < -INTEGRAL_LIMIT) Integral_Roll = -INTEGRAL_LIMIT;
			
			// 计算微分项
			Derivative_Pitch = Error_Pitch_Angle - Last_Error_Pitch;
			Derivative_Roll = Error_Roll_Angle - Last_Error_Roll;
			
			// 保存当前误差作为下一次的历史误差
			Last_Error_Pitch = Error_Pitch_Angle;
			Last_Error_Roll = Error_Roll_Angle;
			
			// 计算PID输出
			PID_Output_Pitch = Kp * Error_Pitch_Angle + Ki * Integral_Pitch + Kd * Derivative_Pitch;
			PID_Output_Roll = Kp * Error_Roll_Angle + Ki * Integral_Roll + Kd * Derivative_Roll;
			
			// PID输出限幅
			if(PID_Output_Pitch > PID_OUTPUT_LIMIT) PID_Output_Pitch = PID_OUTPUT_LIMIT;
			else if(PID_Output_Pitch < -PID_OUTPUT_LIMIT) PID_Output_Pitch = -PID_OUTPUT_LIMIT;
			
			if(PID_Output_Roll > PID_OUTPUT_LIMIT) PID_Output_Roll = PID_OUTPUT_LIMIT;
			else if(PID_Output_Roll < -PID_OUTPUT_LIMIT) PID_Output_Roll = -PID_OUTPUT_LIMIT;
			
			// 计算各电机速度
			// pitch为正时后两轮施加力，roll为正时左两轮施加力
			BR_MotorSpeed = BasicSpeed + BR_BasicSpeed - PID_Output_Pitch + PID_Output_Roll;  // 后右：减Pitch（后端减速）、减Roll（右端减速）
			FR_MotorSpeed = BasicSpeed + FR_BasicSpeed - PID_Output_Pitch - PID_Output_Roll;  // 前右：加Pitch（前端加速）、减Roll（右端减速）
			FL_MotorSpeed = BasicSpeed + FL_BasicSpeed + PID_Output_Pitch - PID_Output_Roll;  // 前左：加Pitch（前端加速）、加Roll（左端加速）
			BL_MotorSpeed = BasicSpeed + BL_BasicSpeed + PID_Output_Pitch + PID_Output_Roll;  // 后左：减Pitch（后端减速）、加Roll（左端加速）		
			
			// 电机速度限幅
			if(BR_MotorSpeed <= 0) BR_MotorSpeed = 0;
			if(FR_MotorSpeed <= 0) FR_MotorSpeed = 0;
			if(FL_MotorSpeed <= 0) FL_MotorSpeed = 0;
			if(BL_MotorSpeed <= 0) BL_MotorSpeed = 0;
			
			// 接收异常或状态不正确时停止电机
			if(NRF24L01_Receive() != 1 || Stutes != 1)
			{
				BR_MotorSpeed = 0;
				FR_MotorSpeed = 0;
				FL_MotorSpeed = 0;
				BL_MotorSpeed = 0;
				// 重置积分项，防止错误累积
				Integral_Pitch = 0;
				Integral_Roll = 0;
			}
		}
		else  // 错误状态，停止所有电机
		{
			BR_MotorSpeed = 0;
			FR_MotorSpeed = 0;
			FL_MotorSpeed = 0;
			BL_MotorSpeed = 0;
			// 重置积分项
			Integral_Pitch = 0;
			Integral_Roll = 0;
		}
		
		// 设置电机速度
		Motor_Setspeed(1, BR_MotorSpeed);  // 后右
		Motor_Setspeed(2, FR_MotorSpeed);  // 前右
		Motor_Setspeed(3, FL_MotorSpeed);  // 前左
		Motor_Setspeed(4, BL_MotorSpeed);  // 后左
		
		// OLED显示
//		OLED_ShowSignedNum(2, 1, BR_MotorSpeed, 2);
//		OLED_ShowSignedNum(2, 4, FR_MotorSpeed, 2);
//		OLED_ShowSignedNum(2, 7, FL_MotorSpeed, 2);
//		OLED_ShowSignedNum(2, 10, BL_MotorSpeed, 2);
		
		OLED_ShowNum(4, 1, NRF24L01_Rx_Packet[0], 3); 
		
		OLED_ShowNum(2,3,FL_BasicSpeed,2);
		OLED_ShowNum(2,8,FR_BasicSpeed,2);
		OLED_ShowNum(3,3,BL_BasicSpeed,2);
		OLED_ShowNum(3,8,BR_BasicSpeed,2);

		
		Delay_ms(5);  // 控制循环周期，影响微分效果
    }
}
