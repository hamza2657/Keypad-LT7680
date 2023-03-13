/********************* COPYRIGHT  **********************
* File Name        : if_port.c
* Author           : Levetop Electronics
* Version          : V1.0
* Date             : 2017-9-11
* Description      : ��ͬ�������ӿ�
********************************************************/

#include "if_port.h"


// ------------------------------------------------------------ Hardware 8-bit SPI driver --------------------------------------------------------------------

#if STM32_SPI_8


uint16_t SPI2_ReadWriteByte(uint16_t TxData)
{		
	//uint16_t retry=0;				 
	while((SPI1->SR&1<<1)==0)		//Wait for the sending area to be empty
	{
//		retry++;
//		if(retry>=0XFFFE)return 0; 	//Exit after timeout
	}			  
	SPI1->DR=TxData;	 	  		// send a byte
//	retry=0;
	while((SPI1->SR&1<<0)==0) 		//Wait to receive a byte
	{
	//	retry++;
//		if(retry>=0XFFFE)return 0;	//Exit after timeout
	}	  						    
	return SPI1->DR;          		//return the received data
}

void SPI_CmdWrite_8(uint8_t cmd)
{
	uint8_t variable_CmdWrite=0;
	SS_RESET; 	      
	SPI2_ReadWriteByte(0x00);

	variable_CmdWrite=SPI2_ReadWriteByte(cmd);
	//printf("myvariable:%x \r\n",variable_CmdWrite);
	SS_SET;
}

void SPI_DataWrite_8(uint8_t data)
{
	SS_RESET; 	      
	SPI2_ReadWriteByte(0x80);
	SPI2_ReadWriteByte(data);
	SS_SET;
}

void SPI_DataWrite_Pixel_8(uint16_t data)
{
	SS_RESET; 	      
	SPI2_ReadWriteByte(0x80);
	SPI2_ReadWriteByte(data);
	SS_SET;
	
	SS_RESET;
	SPI2_ReadWriteByte(0x80);
	SPI2_ReadWriteByte(data>>8);
	SS_SET;
}

uint8_t SPI_StatusRead_8(void)
{
	uint8_t temp = 0;
	uint8_t myvariable=0; //var for checking code working
	SS_RESET; 	
	myvariable=SPI2_ReadWriteByte(0x40);//0X40
	//printf("myvariable:%x \r\n",myvariable);
	temp = SPI2_ReadWriteByte(0xff);
	SS_SET;
	return temp;
}

uint16_t SPI_DataRead_8(void)
{
	uint16_t temp = 0;
	SS_RESET; 	      
	SPI2_ReadWriteByte(0xc0);
	temp = SPI2_ReadWriteByte(0xff);
	SS_SET;
	return temp;
}
#endif


//-----------------------------------------------------------------------------------------------------------------------------------

void LCD_CmdWrite(uint8_t cmd)
{
	
	#if STM32_SPI_8
	SPI_CmdWrite_8(cmd);
	#endif
}

void LCD_DataWrite(uint8_t data)
{
	
	#if STM32_SPI_8
	SPI_DataWrite_8(data);
	#endif
	

}

void LCD_DataWrite_Pixel(uint16_t data)
{

	#if STM32_SPI_8
	SPI_DataWrite_Pixel_8(data);
	#endif
    
}


uint8_t LCD_StatusRead(void)
{
	uint8_t temp = 0;

	
	#if STM32_SPI_8
	temp = SPI_StatusRead_8();
	#endif

	return temp;
}

uint16_t LCD_DataRead(void)
{
	uint16_t temp = 0;
	
	#if STM32_SPI_8
	temp = SPI_DataRead_8();
	#endif
	
	return temp;
}
	  
	 
void Delay_us(uint16_t time)
{    
   uint16_t i=0;  
   while(time--)
   {
      i=12;        //�Լ�����
      while(i--) ;    
   }
}

void Delay_ms(uint16_t time)
{    
   uint16_t i=0;  
   while(time--)
   {
      i=12000;    //�Լ�����
      while(i--) ;    
   }
}


void Parallel_Init(void)
{
	
	#if STM32_SPI_8
		SPI2_Init_8();
	#endif

}
















