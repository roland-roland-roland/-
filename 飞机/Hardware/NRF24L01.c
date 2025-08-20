//飞机NRF24L01

#include "stm32f10x.h"              
#include "NRF24L01_Define.h"           

#define NRF24L01_CSN GPIO_Pin_12	//B
#define NRF24L01_SCK GPIO_Pin_13	//B
#define NRF24L01_MISO GPIO_Pin_14	//B		输入	
#define NRF24L01_MOSI GPIO_Pin_15	//B
#define NRF24L01_IRQ GPIO_Pin_12	//A		输入	
#define NRF24L01_CE GPIO_Pin_11		//A

#define NRF24L01_Rx_Packet_Width	2 	//接收有效载荷宽度
#define NRF24L01_Tx_Packet_Width	1 	//发送有效载荷宽度

uint8_t	NRF24L01_Tx_Packet[NRF24L01_Tx_Packet_Width];
uint8_t	NRF24L01_Rx_Packet[NRF24L01_Rx_Packet_Width];	 

uint8_t NRF24L01_RxAddress[5] = {0x11,0x22,0x33,0x44,0x55}; //接收地址
uint8_t NRF24L01_TxAddress[5] = {0x11,0x22,0x33,0x44,0x55}; //发送地址

void NRF24L01_WriteBit(uint16_t GPIO_Pin,uint8_t BitValue)
{
	if(GPIO_Pin == NRF24L01_CE)
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin,(BitAction)BitValue);
	}
	else
	{
		GPIO_WriteBit(GPIOB,GPIO_Pin,(BitAction)BitValue);
	}
}

uint8_t NRF24L01_ReadBit(uint16_t GPIO_Pin)
{
	return GPIO_ReadInputDataBit(GPIOB,GPIO_Pin); 
}

void NRF24L01_GPIO_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/*********************************************
	GPIOB
	*********************************************/
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = NRF24L01_CSN  | NRF24L01_SCK | NRF24L01_MOSI ;//CSN SCK MOSI
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//输入
	GPIO_InitStructure.GPIO_Pin = NRF24L01_MISO ;		//MISO
 	GPIO_Init(GPIOB, &GPIO_InitStructure);		
	/*********************************************
	GPIOA
	*********************************************/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//输出
	GPIO_InitStructure.GPIO_Pin = NRF24L01_CE;			//CE
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//输入
	GPIO_InitStructure.GPIO_Pin = NRF24L01_IRQ;			//IRQ
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	NRF24L01_WriteBit(NRF24L01_CSN,1);
	NRF24L01_WriteBit(NRF24L01_CE,0);
	NRF24L01_WriteBit(NRF24L01_SCK,0);
	NRF24L01_WriteBit(NRF24L01_MOSI,0);
}

uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte) //Mode0
{
	uint8_t NRF24L01_ReceiveByte = 0;
	uint8_t i;
	for(i=0;i<8;i++)
	{
		NRF24L01_WriteBit(NRF24L01_MOSI,Byte & (0x80 >> i));
		NRF24L01_WriteBit(NRF24L01_SCK,1);
		NRF24L01_ReceiveByte |= (NRF24L01_ReadBit(NRF24L01_MISO) << (7-i));
		NRF24L01_WriteBit(NRF24L01_SCK,0);
	}
	
	return NRF24L01_ReceiveByte;
}

void NRF24L01_WriteReg(uint8_t Reg_Address,uint8_t Data)
{
	NRF24L01_WriteBit(NRF24L01_CSN,0);
	NRF24L01_SPI_SwapByte(NRF24L01_W_REGISTER | Reg_Address);	
	NRF24L01_SPI_SwapByte(Data);	
	NRF24L01_WriteBit(NRF24L01_CSN,1);
}

uint8_t NRF24L01_ReadReg(uint8_t Reg_Address)
{
	uint8_t Data = 0;
	
	NRF24L01_WriteBit(NRF24L01_CSN,0);
	NRF24L01_SPI_SwapByte(NRF24L01_R_REGISTER | Reg_Address);	
	Data = NRF24L01_SPI_SwapByte(NRF24L01_NOP);	
	NRF24L01_WriteBit(NRF24L01_CSN,1);
	return	Data;
}

void NRF24L01_WriteRegs(uint8_t Reg_Address,uint8_t* DataArray,uint8_t Count)
{
	uint8_t i;
	NRF24L01_WriteBit(NRF24L01_CSN,0);
	NRF24L01_SPI_SwapByte(NRF24L01_W_REGISTER | Reg_Address);	
	for(i = 0;i < Count;i ++)
	{
		NRF24L01_SPI_SwapByte(DataArray[i]);
	}	
	NRF24L01_WriteBit(NRF24L01_CSN,1);
}

void NRF24L01_ReadRegs(uint8_t Reg_Address,uint8_t* DataArray,uint8_t Count)
{
	uint8_t i;	
	NRF24L01_WriteBit(NRF24L01_CSN,0);
	NRF24L01_SPI_SwapByte(NRF24L01_R_REGISTER | Reg_Address);	
	for(i = 0;i < Count;i ++)
	{
		DataArray[i] = NRF24L01_SPI_SwapByte(NRF24L01_NOP);	
	}

	NRF24L01_WriteBit(NRF24L01_CSN,1);
}

void NRF24L01_WriteTxPayload(uint8_t* DataArray,uint8_t Count)
{
	uint8_t i;
	NRF24L01_WriteBit(NRF24L01_CSN,0);
	NRF24L01_SPI_SwapByte(NRF24L01_W_TX_PAYLOAD);	
	for(i = 0;i < Count;i ++)
	{
		NRF24L01_SPI_SwapByte(DataArray[i]);
	}	
	NRF24L01_WriteBit(NRF24L01_CSN,1);
}

void NRF24L01_ReadRxPayload(uint8_t* DataArray,uint8_t Count)
{
	uint8_t i;	
	NRF24L01_WriteBit(NRF24L01_CSN,0);
	NRF24L01_SPI_SwapByte(NRF24L01_R_RX_PAYLOAD);	
	for(i = 0;i < Count;i ++)
	{
		DataArray[i] = NRF24L01_SPI_SwapByte(NRF24L01_NOP);	
	}

	NRF24L01_WriteBit(NRF24L01_CSN,1);
}

void NRF24L01_FlushTx(void)
{
	NRF24L01_WriteBit(NRF24L01_CSN,0);
	NRF24L01_SPI_SwapByte(NRF24L01_FLUSH_TX);	
	NRF24L01_WriteBit(NRF24L01_CSN,1);
}

void NRF24L01_FlushRx(void)
{
	NRF24L01_WriteBit(NRF24L01_CSN,0);
	NRF24L01_SPI_SwapByte(NRF24L01_FLUSH_RX);	
	NRF24L01_WriteBit(NRF24L01_CSN,1);
}

uint8_t NRF24L01_ReadStates(void)
{
	uint8_t States;
	NRF24L01_WriteBit(NRF24L01_CSN,0);
	States = NRF24L01_SPI_SwapByte(NRF24L01_NOP);	
	NRF24L01_WriteBit(NRF24L01_CSN,1);
	return States;
}

//NRF24L01功能函数

void NRF24L01_PowerDown(void)
{
	uint8_t Config;
	
	NRF24L01_WriteBit(NRF24L01_CE,0);
	
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	Config &= ~0x02;	//Config 1位置0 其余位不变
	NRF24L01_WriteReg(NRF24L01_CONFIG,Config);
}

void NRF24L01_StandbyI(void)
{
	uint8_t Config;
	
	NRF24L01_WriteBit(NRF24L01_CE,0);
	
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	Config |= 0x02;	//Config 1位置1 其余位不变
	NRF24L01_WriteReg(NRF24L01_CONFIG,Config);
}

void NRF24L01_RxMode(void)
{
	uint8_t Config;
	
	NRF24L01_WriteBit(NRF24L01_CE,0);
	
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	Config |= 0x03;	//Config 1位置1,2位置1 其余位不变
	NRF24L01_WriteReg(NRF24L01_CONFIG,Config);
	
	NRF24L01_WriteBit(NRF24L01_CE,1);
}

void NRF24L01_TxMode(void)
{
	uint8_t Config;
	
	NRF24L01_WriteBit(NRF24L01_CE,0);
	
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	Config |= 0x02;		//Config 1位置1 其余位不变
	Config &= ~0x01;	//Config 0位置0 其余位不变
	NRF24L01_WriteReg(NRF24L01_CONFIG,Config);
	
	NRF24L01_WriteBit(NRF24L01_CE,1);
}

void NRF24L01_Init(void)
{
	NRF24L01_GPIO_Init();
	
	NRF24L01_WriteReg(NRF24L01_CONFIG,0x08);		//配置默认值
	NRF24L01_WriteReg(NRF24L01_EN_AA,0x3F);			//自动应答6通道全开
	NRF24L01_WriteReg(NRF24L01_EN_RXADDR,0x01);		//接收通道仅开启通道0
	NRF24L01_WriteReg(NRF24L01_SETUP_AW,0x03);		//地址宽度5字节
	NRF24L01_WriteReg(NRF24L01_SETUP_RETR,0x03);	//重传延时250us次数3
	NRF24L01_WriteReg(NRF24L01_RF_CH,0X02);			//工作频率2.402GHz
	NRF24L01_WriteReg(NRF24L01_RF_SETUP,0x0E);		//通信功率0dBm通信速率2Mbps
	
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0,NRF24L01_RxAddress,5); //通道0地址
	
	NRF24L01_WriteReg(NRF24L01_RX_PW_P0,NRF24L01_Rx_Packet_Width);		//通道0接收有效载荷宽度
	
	NRF24L01_RxMode(); 	//默认接收模式
}

void NRF24L01_Send(void)
{
	uint8_t Status;
	
	NRF24L01_WriteRegs(NRF24L01_TX_ADDR,NRF24L01_TxAddress,5);  //发送地址
	NRF24L01_WriteTxPayload(NRF24L01_Tx_Packet,NRF24L01_Tx_Packet_Width);	//发送数据与宽度
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0,NRF24L01_TxAddress,5);//在通道0写入发送地址 以便接收应答包(应答包默认发送给发送地址)
	
	NRF24L01_TxMode(); //进入发送模式
	
	while(1)
	{
		Status =  NRF24L01_ReadStates();
		
		if(Status & 0x20)
		{
			//成功
			break;
		}
		else if(Status & 0x10)
		{
			//失败
			break;
		}
		
		//超时退出...
	}
	NRF24L01_WriteReg(NRF24L01_STATUS,0x30);
	
	NRF24L01_FlushTx();		//清空发射区
	
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0,NRF24L01_RxAddress,5);//发送结束后设定接收通道0
	
	NRF24L01_RxMode(); 	//返回接收模式
}

uint8_t NRF24L01_Receive(void)
{
	uint8_t Status;
	
	Status = NRF24L01_ReadStates();
	
	if(Status & 0x40)	//接收标志位
	{	
		NRF24L01_ReadRxPayload(NRF24L01_Rx_Packet,NRF24L01_Rx_Packet_Width);
		
		NRF24L01_WriteReg(NRF24L01_STATUS,0x40);//清除标志位
		
		NRF24L01_FlushRx();
		
		return 1; //收到数据
	}
	return 0; //未收到数据
}
