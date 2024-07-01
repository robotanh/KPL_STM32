/*
 * KeyPad.c
 *
 *  Created on: Jun 27, 2024
 *      Author: Admin
 */


#include "KeyPad.h"

/* Define the key mapping for 4x5 keypad */
//const uint8_t keyMap[4][5] = {
//    {'D', 'C', 'B', 'A', '*'},
//    {'#', '9', '6', '3', '7'},
//    {'0', '8', '5', '2', '4'},
//    {'*', '7', '4', '1', '1'}
//};


const uint8_t keyMap[4][5] = {
    {13, 12, 11, 10,100},
    {200, 9, 6, 3, 7},
    {0, 8, 5, 2, 4},
    {100, 7, 4, 1, 1}
};

#define DEBOUNCE_DELAY 100

GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
uint32_t lastDebounceTime = 0;

/* Initialize the keypad GPIO pins */
void KeyPad_Init(void) {


    /* Configure GPIO pins : PB6 PB7 PB8 PB9 (Rows) */
    GPIO_InitStructPrivate.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStructPrivate.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
    GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructPrivate);

    /* Configure GPIO pins : PB0 PB1 PB2 PB3 PB4 (Columns) */
    GPIO_InitStructPrivate.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
    GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructPrivate.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructPrivate);

    /* Set all rows to high */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);
}

/* Scan the 4x5 keypad and return the pressed key value */
uint8_t KeyPad_Scan(void) {
    uint32_t currentMillis = HAL_GetTick();

    if (currentMillis - lastDebounceTime < DEBOUNCE_DELAY) {
        return 0xFF;
    }

    for (uint8_t row = 0; row < 4; row++) {
        /* Set all rows low */
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);

        /* Set the current row high */
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6 << row, GPIO_PIN_SET);

        /* Read each column */
        for (uint8_t col = 0; col < 5; col++) {
            if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0 << col) == GPIO_PIN_SET) {
                lastDebounceTime = currentMillis;
                return keyMap[row][col];
            }
        }
    }

    return 0xFF;
}
