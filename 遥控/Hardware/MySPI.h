#ifndef __MySPI_H_
#define __MySPI_H_

typedef enum 
{
    SPI_DEVICE_1,  // 用于NRF24L01 SPI1
    SPI_DEVICE_2   // 用于OLED SPI2
} 	SPI_Device;

void MySPI_Init(SPI_Device Device);
void SPI_SendByte(SPI_Device Device, uint8_t Byte);
uint8_t SPI_ReceiveByte(SPI_Device Device);

#endif