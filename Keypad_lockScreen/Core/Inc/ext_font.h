/*
 * font.h
 *
 *  Created on: Oct 19, 2022
 *      Author: fast laptop
 */

#ifndef FONT_H_
#define FONT_H_

void print_User_Define_Ascii(uint16_t x_start, uint16_t y_start, uint32_t Asciiaddr, uint32_t fontcolor, char *c);
static uint16_t LT_Color24_To_16(uint32_t color);

#endif /* LIBRARIES_FONT_FONT_H_ */
