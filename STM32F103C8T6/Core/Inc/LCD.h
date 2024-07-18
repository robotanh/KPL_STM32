/*
 * LCD.h
 *
 *  Created on: Jul 18, 2024
 *      Author: clong
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "main.h"
#include "LED3x6.h"

void ShiftOut_LCD(uint8_t *data, size_t size);

#endif /* INC_LCD_H_ */
