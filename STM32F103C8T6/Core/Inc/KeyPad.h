/*
 * KeyPad.h
 *
 *  Created on: Jun 27, 2024
 *      Author: Admin
 */
#ifndef __KEYPAD_H
#define __KEYPAD_H

#include "stm32f1xx_hal.h"
extern uint8_t keyPressed;

void KeyPad_Init(void);
uint8_t KeyPad_Scan(void);
void KeyLogic();
void KeyLogic_Action();

#endif /* __KEYPAD_H */
