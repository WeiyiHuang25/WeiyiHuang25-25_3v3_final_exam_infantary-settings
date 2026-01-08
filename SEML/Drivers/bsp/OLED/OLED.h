#ifndef __OLED_H
#define __OLED_H
#include "main.h"
#include "SEML_common.h"
#define OLED_I2C_ADDRESS 0x78
#ifdef USE_SEML_LIB
#include "i2c_if.h"
extern I2C_Handle_t OLED_I2C_Handle;
#else

#define USE_HAREWARE_I2C 0
#if USE_HAREWARE_I2C == 1
#define OLED_While(address,data)  HAL_I2C_Mem_Write(&hi2c1,0x78,address,I2C_MEMADD_SIZE_8BIT,data,1,1000)
#else 
/* 引脚定义 */
#define OLED_SCL_PORT       OLED_SCL_GPIO_Port
#define OLED_SCL_PIN        OLED_SCL_Pin
#define OLED_SDA_PORT       OLED_SDA_GPIO_Port
#define OLED_SDA_PIN        OLED_SDA_Pin

/* 宏定义 */
#define OLED_W_SCL(x)   		HAL_GPIO_WritePin(OLED_SCL_PORT, OLED_SCL_PIN, x)
#define OLED_W_SDA(x)		    HAL_GPIO_WritePin(OLED_SDA_PORT, OLED_SDA_PIN, x)
#define OLED_GPIO_Init()    
#endif
#endif
/* 操作函数 */
void OLED_Init(void);                                                                  //OLED初始化
void OLED_Clear(void);                                                                 //OLED清屏
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);                           //OLED显示单个字符
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);                      //OLED显示字符串
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);      //OLED显示无符号数
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length); //OLED显示有符号数
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);   //OLED显示十六进制数
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);   //OLED显示二进制数
void OLED_ShowCHinese16x16(uint8_t x,uint8_t y,uint8_t num,uint8_t (*buf)[16]);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
#endif
