/*
 * LED3x6.c
 *
 *  Created on: Jul 18, 2024
 *      Author: clong
 */
#include "LED3x6.h"
#include "SPI_shift.h"
#include "main.h"



uint8_t digitMapWithOutDP[10] = {
    0b11000000, // 0 without DP
    0b11111001, // 1 without DP
    0b10100100, // 2 without DP
    0b10110000, // 3 without DP
    0b10011001, // 4 without DP
    0b10010010, // 5 without DP
    0b10000010, // 6 without DP
    0b11111000, // 7 without DP
    0b10000000, // 8 without DP
    0b10010000  // 9 without DP
};

uint8_t digitMapWithDP[10] = {
    0b01000000, // 0 with DP
    0b01111001, // 1 with DP
    0b00100100, // 2 with DP
    0b00110000, // 3 with DP
    0b00011001, // 4 with DP
    0b00010010, // 5 with DP
    0b00000010, // 6 with DP
    0b01111000, // 7 with DP
    0b00000000, // 8 with DP
    0b00010000  // 9 with DP
};

uint32_t lcd_num=0;
uint8_t SevenSegScanState=0;
uint32_t SevenSegBuffer[3]={123456, 654321, 987654};

//void ShiftOut(uint8_t data)
//{
//	uint8_t temp;
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
//	for(int i=0;i<8;i++){
//		temp = data & (0x80 >> i);
//		if(temp == 0) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
//		else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
//	}
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
//}
//
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

uint8_t* SevenSegLEDsHandler(uint32_t* buffer, uint8_t scan_state) {
    static uint8_t output[3];
    switch (scan_state) {
        case 0:
            output[0] = buffer[0] % 10;
            output[1] = buffer[1] % 10;
            output[2] = buffer[2] % 10;
            break;
        case 1:
            output[0] = (buffer[0] / 10) % 10;
            output[1] = (buffer[1] / 10) % 10;
            output[2] = (buffer[2] / 10) % 10;
            break;
        case 2:
            output[0] = (buffer[0] / 100) % 10;
            output[1] = (buffer[1] / 100) % 10;
            output[2] = (buffer[2] / 100) % 10;
            break;
        case 3:
            output[0] = (buffer[0] / 1000) % 10;
            output[1] = (buffer[1] / 1000) % 10;
            output[2] = (buffer[2] / 1000) % 10;
            break;
        case 4:
            output[0] = (buffer[0] / 10000) % 10;
            output[1] = (buffer[1] / 10000) % 10;
            output[2] = (buffer[2] / 10000) % 10;
            break;
        case 5:
            output[0] = (buffer[0] / 100000) % 10;
            output[1] = (buffer[1] / 100000) % 10;
            output[2] = (buffer[2] / 100000) % 10;
            break;
    }
    return output;
}


void SevenSegLEDsScan(){
	uint8_t* curr_digit=SevenSegLEDsHandler(SevenSegBuffer,SevenSegScanState);
	uint8_t curr_scan;
	switch (SevenSegScanState) {
		case 0:
			curr_scan=0b00100000;
			SevenSegScanState=1;
			break;
		case 1:
			curr_scan=0b00010000;
			SevenSegScanState=2;
			break;
		case 2:
			curr_scan=0b00001000;
			SevenSegScanState=3;
			break;
		case 3:
			curr_scan=0b00000100;
			SevenSegScanState=4;
			break;
		case 4:
			curr_scan=0b00000010;
			SevenSegScanState=5;
			break;
		case 5:
			curr_scan=0b00000001;
			SevenSegScanState=0;
			break;
		default:
			curr_scan=0b00000001;
			SevenSegScanState=0;
			break;
	}
	uint8_t led_buffer[]={curr_scan,digitMapWithOutDP[curr_digit[2]],digitMapWithOutDP[curr_digit[1]],digitMapWithOutDP[curr_digit[0]]};
	ShiftOut_SPI(led_buffer, 4);
}


