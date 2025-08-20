#ifndef __MPU6050_H__
#define __MPU6050_H__

void MPU6050_Init(void);
void MPU6050_Ware_Write(uint8_t Ware_Address,uint8_t Data);
uint8_t MPU6050_Now_Read(uint8_t Ack);
uint8_t MPU6050_Ware_Read(uint8_t Address);
void MPU6050_GetData(uint8_t *Array);
uint8_t MPU6050_GetID(void);

#endif