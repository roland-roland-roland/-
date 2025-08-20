#include "stm32f10x.h"                  // Device header
#include "Delay.h"

#define LED_GPIO					GPIOB

#define LED1_Pin					GPIO_Pin_9
#define LED2_Pin					GPIO_Pin_1

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin	= LED1_Pin |LED2_Pin;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(LED_GPIO,LED1_Pin|LED2_Pin);
}	

void LED1_ON(void)
{
	GPIO_SetBits(LED_GPIO,LED1_Pin);
}

void LED1_OFF(void)
{
	GPIO_ResetBits(LED_GPIO,LED1_Pin);
}

void LED1_Turn(void)
{
	if(GPIO_ReadOutputDataBit(LED_GPIO,LED1_Pin)==0)
	{GPIO_SetBits(LED_GPIO,LED1_Pin);}
	else
	{GPIO_ResetBits(LED_GPIO,LED1_Pin);}
}

void LED2_ON(void)
{
	GPIO_SetBits(LED_GPIO,LED2_Pin);
}

void LED2_OFF(void)
{
	GPIO_ResetBits(LED_GPIO,LED2_Pin);
}

void LED2_Turn(void)
{
	if(GPIO_ReadOutputDataBit(LED_GPIO,LED2_Pin)==0)
	{GPIO_SetBits(LED_GPIO,LED2_Pin);}
	else
	{GPIO_ResetBits(LED_GPIO,LED2_Pin);}
}

void LED_Flicker(void)
{
	uint8_t i;
	for(i=0;i<9;i++)
	{
		LED1_Turn();
		LED2_Turn();
		Delay_ms(200);
	}
}