#include "stm32f10x.h"                  // Device header
#include "Delay.h"

#define Buzzer_GPIO GPIOB

#define Buzzer_Pin GPIO_Pin_10

void Buzzer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin	= Buzzer_Pin ;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(Buzzer_GPIO,&GPIO_InitStructure);
	
	GPIO_SetBits(Buzzer_GPIO,Buzzer_Pin);
}	

void Buzzer_ON(void)
{
	GPIO_ResetBits(Buzzer_GPIO,Buzzer_Pin);
}

void Buzzer_OFF(void)
{
	GPIO_SetBits(Buzzer_GPIO,Buzzer_Pin);
}

void Buzzer_Turn(void)
{
	if(GPIO_ReadOutputDataBit(Buzzer_GPIO,Buzzer_Pin)==0)
	{GPIO_SetBits(Buzzer_GPIO,Buzzer_Pin);}
	else
	{GPIO_ResetBits(Buzzer_GPIO,Buzzer_Pin);}
}

void Buzzer_Flicker(void)
{
	uint8_t i;
	for(i=0;i<4;i++)
	{
		Buzzer_Turn();
		Delay_ms(100);
	}
}
