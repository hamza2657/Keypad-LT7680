/*
 * font.c
 *
 *  Created on: Oct 19, 2022
 *      Author: fast laptop
 */


#include "stdio.h"
#include "stdint.h"
#include "if_port.h"
#include "LT768.h"
#include "LT768_Lib.h"
#include "main.h"
#include "ext_font.h"







#define W25Q128 1

static void LT_ReadFlash(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
static uint16_t LT_Color24_To_16(uint32_t color);

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

void print_User_Define_Ascii(uint16_t x_start, uint16_t y_start,
		uint32_t Asciiaddr, uint32_t fontcolor, char *c) {
	int SPI_SCS = 1;
		int CLKDVI = 0;
		int TFT_Xsize = 800;

		u8   i = 0;                          //index for the string array
		u8   Ascii_temp;                     //index for the Ascill_code
		u8   font_w[95];
		u8   pngWH[2] = {0};                 //Array for the width and height of the PNG picture

		u16  total_w = x_start;              //X-start coordinate for the String
		u16  y_temp =  y_start;              //Y-start coordinate for the String
		u16  Ascii_w;                        //Width of each PNG picture
		u16  Ascii_h;                        //Height of each PNG picture

		u32 F_color = fontcolor;             //font color

		//Get the width of each character
		//In the Ascii code BIN document, the first two bytes represents the width and height of each PNG picture
		//Starting from the 3rd byte to the 97th byte, each byte represents the effective width of each character
		//Therefore, users can first read the first two bytes to get the width and height of the PNG picture
		//After that, users can futher get the effective width of each ascii code (from 0x20 to 0x7F)
//		 Enable_SFlash_SPI();
//		 Select_SFI_1();
//		 Select_SFI_Dual_Mode0();
//		 SPI_Clock_Period(0);                         // ����SPI�ķ�Ƶϵ��

		LT_ReadFlash(pngWH, Asciiaddr, 2);    //Get the width and height of the PNG picture
		Ascii_w = pngWH[0];                   //width of each PNG picture
		Ascii_h = pngWH[1];                   //height of each PNG picture
		printf("Ascii width = 0x%x\r\n", Ascii_w);
		printf("Ascii Height = 0x%x\r\n", Ascii_h);
		LT_ReadFlash(font_w,Asciiaddr+2,95);  //Get the effective width of each ascill code (0x20 to 0x7F)
		Canvas_Image_Start_address(800*480*2*1);//Setup the canvas address in order ot place the caharacter graphics - 800*480*2 is an example here
		Canvas_image_width (LCD_XSIZE_TFT);   //Setup the canvas width

		//Combine the needed PNG pictures based on the string input by users, and move to designated SDRAM location
		while(c[i] != '\0')
		{
			if(c[i]<=0x7F && c[i]>=0x20)
			{
					Ascii_temp=c[i]-0x20;  //Get the index of the Ascii Code

					//Move the needed png pictures to SDRAM
					//character width:  Ascii_w
			    	//character height: Ascii_h
				    //Address of each character: Asciiaddr + 97 + Ascii_temp * Ascii_w * Ascii_h *2
					uint32_t addr = Asciiaddr+97+Ascii_temp*Ascii_w*Ascii_h*2;
					LT768_DMA_24bit_Block(SPI_SCS, CLKDVI , total_w , y_temp , Ascii_w , Ascii_h , Ascii_w , addr);

					total_w += font_w[Ascii_temp];
					//total_w += 0x1D;
					i++;

			} //Users can add else loop here to deal with except condition.

		}

		//2022_04_24 Print out the Ascii code
		//Each PNG shows a B/W font. Users can further process it by BTE functions to change the font color.
		//Fill the font color to the Ascii code - using BTE_Memory_Copy_Chroma_key function
		Canvas_Image_Start_address(800*480*2*3); //Setup the canvas address in order to fill a Square with indicated color
		Canvas_image_width (LCD_XSIZE_TFT);      //Setup the canvas width

		//F_color = LT_Color24_To_16(fontcolor); //transform the color data from 24bits to 16bits

		//2022_04_24 Clear the display block in MainLayer - 800*480*2*6 is the backup of the mainlayer that is not modified
		LT768_BTE_Memory_Copy_Chroma_key(0, 800, x_start, y_start,  800*480*2*6, 800, x_start, y_start, BGcolor, total_w, total_w);
		//LT768_BTE_Memory_Copy(0, 800, x_start, y_start, 800*480*2*6, 800, x_start, y_start, 0, 800, x_start, y_start, 0x0a, total_w , total_w);//using ROP = 0x0A -> S1

		if(F_color == 0x0000)
		{
			//For balck font, the background color of the PNG picture needs to be transformed to other colors first.
			//1: turn the background color of the PNG to Red  -> make black as the chroma key color, and the font color is still white
			//2: turn the frontground color of the PNG to Black -> make white as the chroma key color, and the font color is transformed to F_color
			LT768_DrawSquare_Fill(x_start, y_start, total_w, (y_start+Ascii_h), Red);
			LT768_BTE_Memory_Copy_Chroma_key(800*480*2, 800, x_start, y_start, 800*480*2*3, TFT_Xsize, x_start, y_start, Black, total_w-x_start, Ascii_h);

			Canvas_Image_Start_address(800*480*2*4);  //Draw a F_color square on another canvas
			Canvas_image_width(LCD_XSIZE_TFT);
			LT768_DrawSquare_Fill(x_start, y_start, total_w, (y_start+Ascii_h), F_color); //Draw and fill the square to designated color: F_color
			LT768_BTE_Memory_Copy_Chroma_key(800*480*2*2, 800, x_start, y_start, 0, TFT_Xsize, x_start, y_start, White, total_w-x_start, Ascii_h);
			LT768_BTE_Memory_Copy_Chroma_key(0, TFT_Xsize, x_start, y_start, 0, TFT_Xsize, x_start, y_start, Black, total_w-x_start, Ascii_h);
		}
		else
		{
			//Steps for font colors other than black color
			//1: turn the frontground (ascii code color) color to the designated color -> The background color is still black
			//2: set Black Chroma_key = black, and only display the ascii code
			LT768_DrawSquare_Fill(x_start, y_start, total_w, (y_start+Ascii_h), fontcolor);
			LT768_BTE_Memory_Copy_Chroma_key(800*480*2*1, 800, x_start, y_start, 800*480*2*3, TFT_Xsize, x_start, y_start, White, total_w-x_start, Ascii_h);
			LT768_BTE_Memory_Copy_Chroma_key(800*480*2*3, TFT_Xsize, x_start, y_start, 0, TFT_Xsize, x_start, y_start, Black, total_w-x_start, Ascii_h);
		}

}







/*
 * Name        ：LT_ReadFlash
 * Functions   ：Read from external Flash
 * Parameters  ：pBuffer: Data to store
 *               ReadAddr:start address(24bit)
 *               NumByteToRead(Max. 65535)
 * Author      ：LEVETOP
 */
static void LT_ReadFlash(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead) {
	//SPI_Clock_Period(1);
#if			W25Q128																															//2019
	LT_W25QXX_Read(pBuffer, ReadAddr, NumByteToRead);
#endif

#if			W25N01G
	LT_W25N01_Read(pBuffer,ReadAddr,NumByteToRead);
#endif																																		  //2019
}

/***************Transfom 24bit color to 16bit color*********************/
static uint16_t LT_Color24_To_16(uint32_t color)
{
	uint16_t r,g,b;
	r = ((color>>16)&0xff)/8;
	g = ((color>>8)&0xff)/4;
	b = ((color>>0)&0xff)/8;

	return ((r<<11)+(g<<5)+b);
}
