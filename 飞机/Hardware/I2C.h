#ifndef __I2C_H__
#define __I2C_H__

void I2C_Soft_Init(void);
void I2C_Start(void);
void I2C_Stop(void);

void I2C_Send_Ack(uint8_t Ack);
void I2C_Send_Byte(uint8_t I2C_Byte);

uint8_t I2C_Receive_Byte(void);
uint8_t I2C_Receive_Ack(void);

#endif
