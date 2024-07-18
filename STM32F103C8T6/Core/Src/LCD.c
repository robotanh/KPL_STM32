///*
// * LCD.c
// *
// *  Created on: Jul 18, 2024
// *      Author: clong
// */
//
//#include "LCD.h"
//
//uint32_t lcd_num=0;
//
//void ShiftOut_LCD(uint8_t *data, size_t size)
//{
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); // Pull STCP (Latch) low
//    HAL_SPI_Transmit(&hspi1, data, size, 300); // Transmit data
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); // Pull STCP (Latch) high
//}
//
//uint8_t* Num_Buff_Conv(uint32_t num) {
//    // Ensure the number is within the valid range
//    if (num > 99999999) {
//        return NULL; // Return NULL for invalid input
//    }
//
//    static uint8_t output[8]; // Static array to hold the result
//    for (int i = 0; i < 8; i++) {
//        output[i] = digitMapWithDP[num % 10]; // Get the least significant digit
//        num /= 10;            // Remove the least significant digit from the number
//    }
//
//    return output;
//}
//
//void Update_LCD(uint32_t num){
//	uint8_t* buffer=Num_Buff_Conv(num);
//	if(buffer==NULL){
//		return;
//	}
//	ShiftOut_LCD(buffer,8);
//}
//
//
