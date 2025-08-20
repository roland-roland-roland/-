#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Motor_Init()
{
	PWM_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_4 |GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void Motor_Setspeed(int8_t Motor,int8_t Speed)
{
	if(Motor == 1)
	{
		PWM_SetCompare1(Speed);
	}
	if(Motor == 2)
	{
		PWM_SetCompare2(Speed);
	}
	if(Motor == 3)
	{
		PWM_SetCompare3(Speed);
	}
	if(Motor == 4)
	{
	   	PWM_SetCompare4(Speed);
	}
}

