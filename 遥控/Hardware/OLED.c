#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "OLED_Font.h"
#include "MySPI.h" 
#include "Delay.h"

// OLED控制引脚定义（仅OLED专用，与SPI分离）
#define OLED_RES_PORT    GPIOB
#define OLED_RES_PIN     GPIO_Pin_14
#define OLED_DC_PORT     GPIOB
#define OLED_DC_PIN      GPIO_Pin_12

// 控制引脚操作宏
#define OLED_RES(x)    GPIO_WriteBit(OLED_RES_PORT, OLED_RES_PIN, (BitAction)(x))
#define OLED_DC(x)     GPIO_WriteBit(OLED_DC_PORT, OLED_DC_PIN, (BitAction)(x))

/**
 * @brief  初始化OLED控制引脚RES/DC
 * @retval 无
 */
static void OLED_ControlPin_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // 使能GPIOB时钟
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = OLED_RES_PIN | OLED_DC_PIN;  // 控制引脚（RES和DC）
    GPIO_Init(OLED_RES_PORT, &GPIO_InitStructure);
}

/**
 * @brief  OLED写命令
 * @param  Command: 要写入的命令
 * @retval 无
 */
static void OLED_WriteCommand(uint8_t Command) {
    OLED_DC(0);  // 命令模式（DC=0）
    SPI_SendByte(SPI_DEVICE_2, Command);  // 使用MySPI2发送（OLED专用SPI设备）
    OLED_DC(1);  // 恢复数据模式（默认）
}

/**
 * @brief  OLED写数据
 * @param  Data: 要写入的数据
 * @retval 无
 */
static void OLED_WriteData(uint8_t Data) {
    OLED_DC(1);  // 数据模式（DC=1）
    SPI_SendByte(SPI_DEVICE_2, Data);  // 使用MySPI2发送
}

/**
 * @brief  OLED设置光标位置
 * @param  Y: 行坐标（0~7，以左上角为原点，向下为正）
 * @param  X: 列坐标（0~127，以左上角为原点，向右为正）
 * @retval 无
 */
static void OLED_SetCursor(uint8_t Y, uint8_t X) {
    OLED_WriteCommand(0xB0 | Y);                  // 设置Y位置（页地址）
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));  // 设置X位置高4位（列地址高位）
    OLED_WriteCommand(0x00 | (X & 0x0F));         // 设置X位置低4位（列地址低位）
}

/**
 * @brief  OLED初始化（含控制引脚和SPI2初始化）
 * @retval 无
 */
void OLED_Init(void)
{
    Delay_ms(10);	//上电保证稳定
	
    OLED_ControlPin_Init();  // 初始化控制引脚（RES和DC）
    MySPI_Init(SPI_DEVICE_2);  // 初始化MySPI2（OLED专用）
    
    // 复位
    OLED_RES(0);
    Delay_ms(10);
    OLED_RES(1);
    
    // OLED初始化命令序列
    OLED_WriteCommand(0xAE);   // 关闭显示
    OLED_WriteCommand(0xD5);   // 设置显示时钟分频比/振荡器频率
    OLED_WriteCommand(0x80);   // 默认值（分频比1，振荡器频率为默认）
    OLED_WriteCommand(0xA8);   // 设置多路复用率
    OLED_WriteCommand(0x3F);   // 64行（0x3F对应64行OLED）
    OLED_WriteCommand(0xD3);   // 设置显示偏移
    OLED_WriteCommand(0x00);   // 无偏移
    OLED_WriteCommand(0x40);   // 设置显示开始行（从第0行开始）
    OLED_WriteCommand(0xA1);   // 设置段重映射（0xA1为正常，0xA0为左右反转）
    OLED_WriteCommand(0xC8);   // 设置COM输出扫描方向（0xC8为正常，0xC0为上下反转）
    OLED_WriteCommand(0xDA);   // 设置COM引脚硬件配置
    OLED_WriteCommand(0x12);   // 序列配置（适用于64行OLED）
    OLED_WriteCommand(0x81);   // 设置对比度控制
    OLED_WriteCommand(0xCF);   // 中等对比度（可根据需求调整）
    OLED_WriteCommand(0xD9);   // 设置预充电周期
    OLED_WriteCommand(0xF1);   // 预充电周期：Phase 1=15 DCLK，Phase 2=1 DCLK
    OLED_WriteCommand(0xDB);   // 设置VCOMH取消选择级别
    OLED_WriteCommand(0x30);   // VCOMH=0.83V
    OLED_WriteCommand(0xA4);   // 显示全部点亮（0xA4为正常显示，0xA5为强制全亮）
    OLED_WriteCommand(0xA6);   // 正常显示（0xA6为正常，0xA7为反显）
    OLED_WriteCommand(0x8D);   // 开启电荷泵
    OLED_WriteCommand(0x14);   // 电荷泵使能（0x14为开启，0x10为关闭）
    OLED_WriteCommand(0xAF);   // 开启显示
    
    OLED_Clear();  // 初始化完成后清屏
}

/**
 * @brief  OLED清屏
 * @retval 无
 */
void OLED_Clear(void) 
{  
    uint8_t i, j;
    for (j = 0; j < 8; j++)  // 循环8页（共64行）
    {
        OLED_SetCursor(j, 0);  // 定位到第j页第0列
        for(i = 0; i < 128; i++)  // 每列写入0x00（熄灭）
        {
            OLED_WriteData(0x00);
        }
    }
}

/**
 * @brief  OLED显示一个字符
 * @param  Line: 行位置（1~4，共4行）
 * @param  Column: 列位置（1~16，共16列）
 * @param  Char: 要显示的字符（ASCII可见字符）
 * @retval 无
 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char) 
{      	
    uint8_t i;
    // 定位到字符上半部分（8行）
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_F8x16[Char - ' '][i]);  // 显示上半部分点阵
    }
    // 定位到字符下半部分（8行）
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);  // 显示下半部分点阵
    }
}

/**
 * @brief  OLED显示字符串
 * @param  Line: 起始行位置（1~4）
 * @param  Column: 起始列位置（1~16）
 * @param  String: 要显示的字符串（ASCII可见字符，以'\0'结尾）
 * @retval 无
 */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)  // 循环到字符串结束
    {
        OLED_ShowChar(Line, Column + i, String[i]);  // 逐个显示字符
    }
}

/**
 * @brief  次方函数（内部辅助）
 * @param  X: 底数
 * @param  Y: 指数（Y≥0）
 * @retval X的Y次方结果
 */
uint32_t OLED_Pow(uint32_t X, uint32_t Y) 
{
    uint32_t Result = 1;
    while (Y--)  // 循环Y次，每次乘以X
    {
        Result *= X;
    }
    return Result;
}

/**
 * @brief  OLED显示无符号十进制数字
 * @param  Line: 起始行位置（1~4）
 * @param  Column: 起始列位置（1~16）
 * @param  Number: 要显示的数字（0~4294967295）
 * @param  Length: 要显示的数字长度（1~10）
 * @retval 无
 */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)  // 循环显示每一位
    {
        // 提取第i位数字（从高位到低位），转换为字符并显示
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
 * @brief  OLED显示带符号十进制数字
 * @param  Line: 起始行位置（1~4）
 * @param  Column: 起始列位置（1~16）
 * @param  Number: 要显示的数字（-2147483648~2147483647）
 * @param  Length: 要显示的数字长度（1~10，不含符号位）
 * @retval 无
 */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length) {
    uint8_t i;
    uint32_t Number1;  // 绝对值
    
    if (Number >= 0)  // 正数
    {
        OLED_ShowChar(Line, Column, '+');
        Number1 = Number;  // 直接取原值
    }
    else  // 负数
    {
        OLED_ShowChar(Line, Column, '-');
        Number1 = (uint32_t)(-Number);  // 注意：-2147483648的绝对值超出int32_t范围，但实际显示时Length足够则可正常显示
    }
    
    // 循环显示每一位数字（从符号位后开始）
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
 * @brief  OLED显示十六进制无符号数字
 * @param  Line: 起始行位置（1~4）
 * @param  Column: 起始列位置（1~16）
 * @param  Number: 要显示的数字（0~0xFFFFFFFF）
 * @param  Length: 要显示的数字长度（1~8）
 * @retval 无
 */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i, SingleNumber;  // SingleNumber：每一位十六进制数（0~15）
    for (i = 0; i < Length; i++)
    {
        // 提取第i位十六进制数（从高位到低位）
        SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
        
        if (SingleNumber < 10)  // 0~9：直接转换为字符'0'~'9'
        {
            OLED_ShowChar(Line, Column + i, SingleNumber + '0');
        }
        else  // 10~15：转换为字符'A'~'F'
        {
            OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
        }
    }
}

/**
 * @brief  OLED显示二进制无符号数字
 * @param  Line: 起始行位置（1~4）
 * @param  Column: 起始列位置（1~16）
 * @param  Number: 要显示的数字（0~0xFFFF）
 * @param  Length: 要显示的数字长度（1~16）
 * @retval 无
 */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)  // 循环显示每一位
    {
        // 提取第i位二进制数（从高位到低位），转换为字符'0'或'1'
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
    }
}