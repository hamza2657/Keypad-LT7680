#ifndef _TOUCH_GTXXXX_H
#define _TOUCH_GTXXXX_H

#define LCD_PIXEL_WIDTH  800
#define LCD_PIXEL_HEIGHT 480
#define CTP_ADDRESS      0xBA //0xBA or 0x28
#define CTP_MAX_TOUCH    1 //1-10
																		 
int32_t CTP_Init_Panel(void);

void CTP_TS_Work_Func(); 

uint8_t CTP_read(uint16_t* x, uint16_t* y, uint8_t* pressed);
typedef struct {
	uint32_t input_x;
	uint32_t input_y;
	uint32_t input_w;
	}point_coor;

	extern point_coor point;
#endif /*_TOUCH_GTXXXX_H */
