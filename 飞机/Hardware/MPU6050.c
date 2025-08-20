#include "stm32f10x.h"                  // Device header
#include "MPU6050_REG.h"

#define MPU6050_Address	0xD0
#define MPU6050_Address_Write 0xD0
#define MPU6050_Address_Read 0xD1

void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Tinmeout = 10000;

	while (I2C_CheckEvent(I2Cx,I2C_EVENT) !=  SUCCESS)
	{
		Tinmeout -- ;
		if(Tinmeout == 0)
		{
			break;
		}
	} 
}

void MPU6050_Ware_Write(uint8_t Ware_Address,uint8_t Data)
{
	I2C_GenerateSTART(I2C1,ENABLE);
	MPU6050_WaitEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C1,MPU6050_Address,I2C_Direction_Transmitter);
	MPU6050_WaitEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	I2C_SendData(I2C1,Ware_Address);
	MPU6050_WaitEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	
	I2C_SendData(I2C1,Data);
	MPU6050_WaitEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	I2C_GenerateSTOP(I2C2,ENABLE);
	
}

uint8_t MPU6050_Ware_Read(uint8_t Address)
{
	uint8_t Data;
	
	I2C_GenerateSTART(I2C1,ENABLE);
	MPU6050_WaitEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C1,MPU6050_Address,I2C_Direction_Transmitter);
	MPU6050_WaitEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	I2C_SendData(I2C1,Address);
	MPU6050_WaitEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	
	I2C_GenerateSTART(I2C1,ENABLE);
	MPU6050_WaitEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C1,MPU6050_Address,I2C_Direction_Receiver);
	MPU6050_WaitEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
	
	I2C_AcknowledgeConfig(I2C1,DISABLE);
	I2C_GenerateSTOP(I2C1,ENABLE);
	
	MPU6050_WaitEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED);
	Data = I2C_ReceiveData(I2C1);
	
	I2C_AcknowledgeConfig(I2C1,ENABLE);
	
	return Data;
}

void MPU6050_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_6 |GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_ClockSpeed = 50000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;		
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2C1,&I2C_InitStructure);
	
	I2C_Cmd(I2C1,ENABLE);
	
	MPU6050_Ware_Write(MPU6050_PWR_MGMT_1,0x01);
	MPU6050_Ware_Write(MPU6050_PWR_MGMT_2,0x00);
	MPU6050_Ware_Write(MPU6050_SMPLRT_DIV,0x09);
	MPU6050_Ware_Write(MPU6050_CONFIG,0x06);
	MPU6050_Ware_Write(MPU6050_GYRO_CONFIG,0x18);
	MPU6050_Ware_Write(MPU6050_ACCEL_CONFIG,0x18);
}

void MPU6050_GetData(uint8_t *Array)
{	
	Array[0] = MPU6050_Ware_Read(MPU6050_ACCEL_XOUT_H);
	Array[1] = MPU6050_Ware_Read(MPU6050_ACCEL_XOUT_L);
	
	Array[2] = MPU6050_Ware_Read(MPU6050_ACCEL_YOUT_H);
	Array[3] = MPU6050_Ware_Read(MPU6050_ACCEL_YOUT_L);

	Array[4] = MPU6050_Ware_Read(MPU6050_ACCEL_ZOUT_H);
	Array[5] = MPU6050_Ware_Read(MPU6050_ACCEL_ZOUT_L);
	
	Array[6] = MPU6050_Ware_Read(MPU6050_GYRO_XOUT_H);
	Array[7] = MPU6050_Ware_Read(MPU6050_GYRO_XOUT_L);
	
	Array[8] = MPU6050_Ware_Read(MPU6050_GYRO_YOUT_H);
	Array[9] = MPU6050_Ware_Read(MPU6050_GYRO_YOUT_L);
	
	Array[10] = MPU6050_Ware_Read(MPU6050_GYRO_ZOUT_H);
	Array[11] = MPU6050_Ware_Read(MPU6050_GYRO_ZOUT_L);
}

uint8_t MPU6050_GetID(void)
{
	return MPU6050_Ware_Read(MPU6050_WHO_AM_I);
}