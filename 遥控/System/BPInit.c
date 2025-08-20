#include "stm32f10x.h"                  // Device header
#include "LED.h"
#include "OLED.h"
#include "Buzzer.h"

void BP_Init(void)
{
	OLED_Init();
	Buzzer_Init();
	LED_Init();
	OLED_Clear();
}