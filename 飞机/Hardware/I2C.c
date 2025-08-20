#include "stm32f10x.h"                  // Device header
#include <Delay.h> 

#define I2C_SCL_Pin GPIO_Pin_6
#define I2C_SDA_Pin GPIO_Pin_7
#define I2C_GPIO 	GPIOB

void I2C_SCL(uint8_t x)
{
	GPIO_WriteBit(I2C_GPIO, I2C_SCL_Pin, (BitAction)(x));
	Delay_us(10);
}

void I2C_SDA(uint8_t x)
{
	GPIO_WriteBit(I2C_GPIO, I2C_SDA_Pin, (BitAction)(x));
	Delay_us(10);
}

void I2C_Soft_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  	//发送模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = I2C_SCL_Pin;
 	GPIO_Init(I2C_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = I2C_SDA_Pin;
 	GPIO_Init(I2C_GPIO, &GPIO_InitStructure);
}

void I2C_Start(void)
{
	I2C_SDA(1);
	I2C_SCL(1);
	I2C_SDA(0);
	I2C_SCL(0);
}

void I2C_Stop(void)
{
	I2C_SDA(0);
	I2C_SCL(1);
	I2C_SDA(1);
}

void I2C_Send_Bit(uint8_t I2C_Bit)
{
	I2C_SCL(0);
	I2C_SDA(I2C_Bit);
	I2C_SCL(1);
	I2C_SCL(0);
}

uint8_t I2C_Receive_Bit(void)
{
	uint8_t I2C_Bit;
	I2C_SDA(1); 
	I2C_SCL(1);
	I2C_Bit = GPIO_ReadInputDataBit(I2C_GPIO,I2C_SDA_Pin);
	Delay_us(10);
	I2C_SCL(0);
	return I2C_Bit;
}

void I2C_Send_Byte(uint8_t I2C_Byte)
{
	uint8_t I2C_Bit,i;
	for(i=0;i<8;i++)
	{
		I2C_Send_Bit(I2C_Byte >> (7 - i) & 0x01);
	}
}

uint8_t I2C_Receive_Byte(void)
{
	uint8_t I2C_Byte = 0x00;
	uint8_t i;
	for(i=0;i<8;i++)
	{
		I2C_Byte |= ((I2C_Receive_Bit() & 0x01) << (7 - i));
	}
	return I2C_Byte;
}

void I2C_Send_Ack(uint8_t Ack)
{
	I2C_Send_Bit(Ack);
}


uint8_t I2C_Receive_Ack(void)
{
	uint8_t Ack;
	Ack = I2C_Receive_Bit();
	return Ack;
}
