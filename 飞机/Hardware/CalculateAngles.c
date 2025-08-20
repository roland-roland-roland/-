#include "stm32f10x.h"                  // Device header
#include "math.h"  // 用于atan2函数
#include "NRF24L01.h"
#include "OLED.h"
#include "MPU6050.h"

#define M_PI 3.14f

int16_t AX, AY, AZ, GX, GY, GZ;
int16_t pitch, roll;  // 俯仰角和横滚角（整数，单位：度）
uint8_t OriginialArray[12];
uint8_t ErrorStutes;

// 计算角度的函数并显示
void CalculateAngles(void) 
{
	MPU6050_GetData(OriginialArray);
	
	AX = (OriginialArray[0] << 8) | OriginialArray[1];
	AX = AX * 10000 / 2048; 
	AY = (OriginialArray[2] << 8) | OriginialArray[3];
	AY = AY * 10000 / 2048;
	AZ = (OriginialArray[4] << 8) | OriginialArray[5];
	AZ = AZ * 10000 / 2048;
	GX = (OriginialArray[6] << 8) | OriginialArray[7];
	GX = GX * 10000 / 2048;
	GY = (OriginialArray[8] << 8) | OriginialArray[9];
	GY = GY * 10000 / 2048;
	GZ = (OriginialArray[10] << 8) | OriginialArray[11];
	GZ = GZ * 10000 / 2048;

	/*********** 计算角度 *********/
	// 避免除零错误
	if (AZ == 0) {
		pitch = (AY > 0) ? 90 : -90;
		roll = (AX > 0) ? -90 : 90;
		return;
	}

	// 计算Pitch角：arctan(AY/AZ) * 180/π
	// 先转换为浮点数计算，再转换为整数度
	float pitch_float = atan2((float)AY / 10000.0f, (float)AZ / 10000.0f) * 180.0f / M_PI;
	pitch = (int16_t)(pitch_float + 0.5f);  // 四舍五入

	// 计算Roll角（绕Y轴）：arctan(-AX/AZ) * 180/π
	float roll_float = atan2(-(float)AX / 10000.0f, (float)AZ / 10000.0f) * 180.0f / M_PI;
	roll = (int16_t)(roll_float + 0.5f);  // 四舍五入

	if((pitch >= 50||pitch <= -50)&&(roll >= 50||roll <= -50))
	{
		ErrorStutes = 1;
	}
}

