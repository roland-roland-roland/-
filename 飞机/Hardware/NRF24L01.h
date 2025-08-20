#ifndef __NRF24L01_H__
#define __NRF24L01_H__

#include "NRF24L01_Define.h"  

extern uint8_t	NRF24L01_Tx_Packet[];
extern uint8_t	NRF24L01_Rx_Packet[];	 

void NRF24L01_WriteBit(uint16_t GPIO_Pin,uint8_t BitValue);
void NRF24L01_GPIO_Init();
uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte); //Mode0
void NRF24L01_WriteReg(uint8_t Reg_Address,uint8_t Data);
uint8_t NRF24L01_ReadReg(uint8_t Reg_Address);
void NRF24L01_WriteRegs(uint8_t Reg_Address,uint8_t* DataArray,uint8_t Count);
void NRF24L01_ReadRegs(uint8_t Reg_Address,uint8_t* DataArray,uint8_t Count);

void NRF24L01_Init(void);
uint8_t NRF24L01_Receive(void);
void NRF24L01_Send(void);

#endif