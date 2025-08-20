#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "NRF24L01.h"
#include "Key.h" 
#include "Timer.h" 	
#include "LED.h"
#include "Buzzer.h"
#include "AD.h"

uint8_t KeyNum;
uint16_t Joystick_Array[4];

int main(void)
{	
	
	/******************************
	初始化
	******************************/
	OLED_Init();
	NRF24L01_Init();
	Key_Init();
	Timer_Init();
	LED_Init();
	Buzzer_Init();
	AD_Init();
	
	
	LED_Flicker();
	// Buzzer_Flicker();
	
    while(1)
    {
		KeyNum = Key_GetNum();
		AD_GetJoystick(Joystick_Array);
		OLED_ShowNum(1,1,Joystick_Array[3],4);
		NRF24L01_Tx_Packet[0] = Joystick_Array[3] / 0.4096 / 100;
		if(KeyNum == 7)
		{
			NRF24L01_Tx_Packet[1] = 1;
		}
		if(KeyNum == 8)
		{
			NRF24L01_Tx_Packet[1] = 0;
		}
		OLED_ShowNum(2,1,NRF24L01_Tx_Packet[0],4);
		OLED_ShowNum(3,1,NRF24L01_Tx_Packet[1],4);
		NRF24L01_Send();
		Delay_ms(10);
    }
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		Key_Tick();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
