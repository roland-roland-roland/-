#include "stm32f10x.h"                  // Device header
#include "Delay.h"

#define Key_Port GPIOB

#define Key_Left GPIO_Pin_8
#define Key_Right GPIO_Pin_7
#define Key_Right_Trim GPIO_Pin_6
#define Key_Behind_Trim GPIO_Pin_5
#define Key_Left_Trim GPIO_Pin_4
#define Key_Front_Trim GPIO_Pin_3

uint8_t Key_Num = 0;		

void Key_Init(void)
{
	//涉及复用释放PB3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//释放复用PB3
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
		
	
	//GPIO初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = Key_Front_Trim | Key_Left_Trim | 
								  Key_Behind_Trim | Key_Right_Trim | 
								  Key_Right | Key_Left;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Key_Port, &GPIO_InitStructure);				

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

uint8_t Key_GetNum(void)
{
	uint8_t Temp;
	if(Key_Num)
	{
		Temp = Key_Num;
		Key_Num = 0;
		return Temp;
	}
	return 0;	
}

uint8_t Key_GetState(void)
{
    if(GPIO_ReadInputDataBit(Key_Port, Key_Front_Trim) == 0) return 3;
    if(GPIO_ReadInputDataBit(Key_Port, Key_Left_Trim)  == 0) return 4;
    if(GPIO_ReadInputDataBit(Key_Port, Key_Behind_Trim)== 0) return 5;
    if(GPIO_ReadInputDataBit(Key_Port, Key_Right_Trim) == 0) return 6;
    if(GPIO_ReadInputDataBit(Key_Port, Key_Right)      == 0) return 7;
    if(GPIO_ReadInputDataBit(Key_Port, Key_Left)       == 0) return 8;
    return 0;
}

void Key_Tick(void)
{
	static uint8_t Count;
	static uint8_t CurrState,PrevState;
	Count ++;
	if(Count >= 20)
	{
		Count = 0;
		PrevState = CurrState;
		CurrState = Key_GetState();
		
		if(CurrState == 0 && PrevState != 0)
		{
			Key_Num = PrevState;
		}
	}
	
}