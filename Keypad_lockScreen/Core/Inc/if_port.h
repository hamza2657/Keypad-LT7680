/********************* COPYRIGHT  **********************
* File Name        : if_port.h
* Author           : Levetop Electronics
* Version          : V1.0
* Date             : 2017-9-11
* Description      : Choose a different driver interface
********************************************************/

#ifndef _if_port_h
#define _if_port_h
#include "sys.h"      // header file for STM32
#include "stdio.h"
#include "main.h"

//-------- Only one peripheral can be enabled ----------

#define STM32_SPI_8   1     // Use STM32's SPI to drive LT768 (hardware 8-bit)



//Select SPI driver
#if STM32_SPI_8 | STM32_SPI_16

#define SS_RESET     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET)
#define SS_SET       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET)

//#define SS_RESET      (GPIOA->ODR &= ~(1<<15))
//#define SS_SET       (GPIOA->ODR |= (1<<15))
uint16_t SPI2_ReadWriteByte(uint16_t TxData);
#endif


void Parallel_Init(void);

void LCD_CmdWrite(uint8_t cmd);
void LCD_DataWrite(uint8_t data);
void LCD_DataWrite_Pixel(uint16_t data);
uint8_t LCD_StatusRead(void);
uint16_t LCD_DataRead(void);
	 
void Delay_us(uint16_t time);     // Delay function us level
void Delay_ms(uint16_t time);     // Delay function ms level






#endif
