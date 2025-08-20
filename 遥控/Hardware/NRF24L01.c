//遥控器NRF24L01
#include "stm32f10x.h"              
#include "NRF24L01_Define.h"           
#include "OLED.h"
#include "Delay.h"

#define NRF24L01_Port GPIOA

#define NRF24L01_CSN GPIO_Pin_4
#define NRF24L01_SCK GPIO_Pin_5
#define NRF24L01_MISO GPIO_Pin_6	//输入
#define NRF24L01_MOSI GPIO_Pin_7
#define NRF24L01_IRQ GPIO_Pin_8		//输入
#define NRF24L01_CE GPIO_Pin_15		

#define NRF24L01_Rx_Packet_Width	1	//接收有效载荷宽度
#define NRF24L01_Tx_Packet_Width	2 	//发送有效载荷宽度

uint8_t	NRF24L01_Tx_Packet[NRF24L01_Tx_Packet_Width];
uint8_t	NRF24L01_Rx_Packet[NRF24L01_Rx_Packet_Width];	 

uint8_t NRF24L01_RxAddress[5] = {0x11,0x22,0x33,0x44,0x55}; //接收地址
uint8_t NRF24L01_TxAddress[5] = {0x11,0x22,0x33,0x44,0x55}; //发送地址

void NRF24L01_WriteBit(uint16_t GPIO_Pin,uint8_t BitValue)
{
	GPIO_WriteBit(NRF24L01_Port,GPIO_Pin,(BitAction)BitValue);
}

uint8_t NRF24L01_ReadBit(uint16_t GPIO_Pin)
{
	return GPIO_ReadInputDataBit(NRF24L01_Port,GPIO_Pin); 
}

void NRF24L01_GPIO_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // 开启AFIO时钟
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  // 禁用JTAG，保留SWD（方便调试）
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = NRF24L01_CSN  | NRF24L01_SCK 
								| NRF24L01_MOSI | NRF24L01_CE;
 	GPIO_Init(NRF24L01_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = NRF24L01_MISO | NRF24L01_IRQ;
 	GPIO_Init(NRF24L01_Port, &GPIO_InitStructure);		
	
	NRF24L01_WriteBit(NRF24L01_CSN,1);
	NRF24L01_WriteBit(NRF24L01_CE,0);
	NRF24L01_WriteBit(NRF24L01_SCK,0);
	NRF24L01_WriteBit(NRF24L01_MOSI,0);
}

uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte) //Mode0 修复时序
{
    uint8_t NRF24L01_ReceiveByte = 0;
    uint8_t i;
    for(i=0;i<8;i++)
    {
        // 1. SCK低电平：更新MOSI数据（提前准备好）
        NRF24L01_WriteBit(NRF24L01_MOSI, Byte & (0x80 >> i));
        // （可选）加短延时，确保MOSI稳定（时钟72MHz时需加）
        Delay_us(1);  // 需实现delay_us函数（基于SysTick）
        
        // 2. SCK拉高：模块采样MISO数据
        NRF24L01_WriteBit(NRF24L01_SCK, 1);
        Delay_us(1);  // 确保采样时间
        NRF24L01_ReceiveByte |= (NRF24L01_ReadBit(NRF24L01_MISO) << (7-i));
        
        // 3. SCK拉低：准备下一位
        NRF24L01_WriteBit(NRF24L01_SCK, 0);
        Delay_us(1);
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
    
    NRF24L01_WriteBit(NRF24L01_CE, 0);  // CE先拉低
    Delay_us(20);  // 确保CE稳定低电平
    
    Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
    Config |= 0x03;  // PWR_UP=1（bit1），PRIM_RX=1（bit0）
    NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
    
    NRF24L01_WriteBit(NRF24L01_CE, 1);  // CE拉高，进入接收模式
    Delay_us(150);  // 保持≥130us，确保模块稳定接收
}

void NRF24L01_TxMode(void)
{
	uint8_t Config;
	
	NRF24L01_WriteBit(NRF24L01_CE,0);
	
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	Config |= 0x02;		//Config 1位置1 其余位不变
	Config &= ~0x01;	//Config 0位置0 其余位不变
	NRF24L01_WriteReg(NRF24L01_CONFIG,Config);
	
	NRF24L01_WriteBit(NRF24L01_CE, 1);  // CE拉高，进入发送模式
    Delay_us(20);  // 保持CE高电平≥10us，确保触发发送
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
    uint32_t Timeout = 0;  // 关键：定义在循环外，初始化计数
    
    NRF24L01_WriteRegs(NRF24L01_TX_ADDR, NRF24L01_TxAddress, 5);  
    NRF24L01_WriteTxPayload(NRF24L01_Tx_Packet, NRF24L01_Tx_Packet_Width);	
    NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_TxAddress, 5);
    
    NRF24L01_TxMode(); 
    
    while(1)
    {
        Status = NRF24L01_ReadStates();
        
        if(Status & 0x20)  // 发送成功（TX_DS）
        {
            OLED_ShowString(3,1,"SUCCESS");
            break;
        }
        else if(Status & 0x10)  // 发送失败（MAX_RT，重传次数用尽）
        {
            OLED_ShowString(4,1,"FAIL");
            break;
        }
        
        Timeout++;
        if(Timeout > 72000)  // 72MHz时钟下约1ms超时
        {
            OLED_ShowString(4,6,"FUCK");
            break;
        }
    }
    NRF24L01_WriteReg(NRF24L01_STATUS, 0x30);  // 清除TX_DS和MAX_RT标志
    NRF24L01_FlushTx();		
    NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_RxAddress, 5);
    NRF24L01_RxMode(); 	
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
