#include "stm32f10x.h"                  // Device header
#include "Delay.h"

/*********************************************
	LED低电平点亮
*********************************************/
	
#define LED_GPIO					GPIOB

#define LED1_Pin					GPIO_Pin_9
#define LED2_Pin					GPIO_Pin_8
#define LED3_Pin					GPIO_Pin_2
#define LED4_Pin					GPIO_Pin_1


void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin	= LED1_Pin | LED2_Pin | LED3_Pin | LED4_Pin;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(LED_GPIO,LED1_Pin | LED2_Pin | LED3_Pin | LED4_Pin);
}	


void LED_Flicker(void)
{
	uint8_t i;
	for(i=0;i<4;i++)
	{
		GPIO_ResetBits(LED_GPIO,LED1_Pin | LED2_Pin | LED3_Pin | LED4_Pin);
		Delay_ms(200);
		GPIO_SetBits(LED_GPIO,LED1_Pin | LED2_Pin | LED3_Pin | LED4_Pin);
		Delay_ms(200);
	}
}