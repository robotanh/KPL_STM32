/*
 * SPI_shift.c
 *
 *  Created on: Jul 18, 2024
 *      Author: clong
 */
#include "SPI_shift.h"

void ShiftOut_SPI(uint8_t *data, size_t size)
{

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); // Pull STCP (Latch) low
    HAL_SPI_Transmit(&hspi2, data, size, 300); // Transmit data
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); // Pull STCP (Latch) high

}

