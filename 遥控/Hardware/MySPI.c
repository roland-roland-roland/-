#include "stm32f10x.h"                  // Device header
#include "MySPI.h"

// SPI1引脚定义		NRF24L01
#define SPI1_SCK_GPIO_PORT    GPIOA
#define SPI1_SCK_GPIO_PIN     GPIO_Pin_5
#define SPI1_MOSI_GPIO_PORT   GPIOA
#define SPI1_MOSI_GPIO_PIN    GPIO_Pin_7
#define SPI1_MISO_GPIO_PORT   GPIOA
#define SPI1_MISO_GPIO_PIN    GPIO_Pin_6

// SPI2引脚定义		OLED12864
#define SPI2_SCK_GPIO_PORT    GPIOB
#define SPI2_SCK_GPIO_PIN     GPIO_Pin_13
#define SPI2_MOSI_GPIO_PORT   GPIOB
#define SPI2_MOSI_GPIO_PIN    GPIO_Pin_15

/**
 * @brief  获取SPI外设指针
 * @param  Device: SPI设备
 * @retval SPI外设指针（SPI1或SPI2）
 */
static SPI_TypeDef* SPI_GetPeriph(SPI_Device Device) 
{
	if(Device == SPI_DEVICE_1)
	{
		return SPI1;
	}
    else
	{
		return SPI2;
	}
}

/**
 * @brief  初始化SPI1	NRF24L01
 * 配置：全双工、主机模式、8位数据、CPOL=Low、CPHA=1Edge（NRF24L01兼容模式）
 */
static void SPI1_Init(void) {
   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;
    
    // 配置SCK和MOSI为复用推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = SPI1_SCK_GPIO_PIN | SPI1_MOSI_GPIO_PIN;
    GPIO_Init(SPI1_SCK_GPIO_PORT, &GPIO_InitStructure);
    
    // 配置MISO为上拉输入（NRF24L01需要接收数据）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = SPI1_MISO_GPIO_PIN;
    GPIO_Init(SPI1_MISO_GPIO_PORT, &GPIO_InitStructure);
    
    // SPI1参数配置
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // 全双工（NRF需要接收）
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                         // 时钟极性：低电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                       // 时钟相位：第一个边沿
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                          // 软件控制片选
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;  // 波特率分频（72MHz/8=9MHz）
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                  // 高位在前
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);
    
    SPI_Cmd(SPI1, ENABLE);
}

/**
 * @brief  初始化SPI2	OLED12864
 * 配置：单线发送、主机模式、8位数据、CPOL=Low、CPHA=1Edge（OLED兼容模式）
 */
static void SPI2_Init(void) 
{
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;
    
    // 配置SCK和MOSI为复用推挽输出（OLED只需要发送）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = SPI2_SCK_GPIO_PIN | SPI2_MOSI_GPIO_PIN;
    GPIO_Init(SPI2_SCK_GPIO_PORT, &GPIO_InitStructure);
    
    // SPI2参数配置
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;  // 单线发送（OLED无需接收）
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                 // 时钟极性：低电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;               // 时钟相位：第一个边沿
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                  // 软件控制片选
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;  // 72MHz/8=9MHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);
    
    SPI_Cmd(SPI2, ENABLE);
}

/**
 * @brief  初始化指定SPI设备
 * @param  Device: 选择SPI设备（SPI_DEVICE_1或SPI_DEVICE_2）
 * @retval 无
 */
void MySPI_Init(SPI_Device Device) 
{
    if (Device == SPI_DEVICE_1){SPI1_Init();}
	else{SPI2_Init();}
}

/**
 * @brief  通过指定SPI设备发送一个字节
 * @param  Device: 选择SPI设备（SPI_DEVICE_1或SPI_DEVICE_2）
 * @param  Byte: 要发送的字节
 * @retval 无
 */
void SPI_SendByte(SPI_Device Device, uint8_t Byte) 
{
    SPI_TypeDef* SPIx = SPI_GetPeriph(Device);
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);  // 等待发送缓冲区空
    SPI_I2S_SendData(SPIx, Byte);
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET);    // 等待发送完成
}

/**
 * @brief  通过指定SPI设备接收一个字节（用于NRF24L01等需要接收的设备）
 * @param  Device: 选择SPI设备（SPI_DEVICE_1或SPI_DEVICE_2）
 * @retval 接收的字节
 */
uint8_t SPI_ReceiveByte(SPI_Device Device)
{
    SPI_TypeDef* SPIx = SPI_GetPeriph(Device);
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);  // 等待发送缓冲区空
    SPI_I2S_SendData(SPIx, 0xFF);  // 发送 dummy 数据触发接收
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET); // 等待接收完成
    return SPI_I2S_ReceiveData(SPIx);
}


