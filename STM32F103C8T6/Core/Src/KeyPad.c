#include "KeyPad.h"
#include "main.h"


const uint8_t keyMap[4][5] = {
    {'C', '7', '4', '1', 'A'},
    {'0', '8', '5', '2', 'B'},
    {'E', '9', '6', '3', 'D'},
	{'T', 'P', '$', 'L', 'F'}
};

#define DEBOUNCE_DELAY 500
#define HOLD_DELAY 3000

GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
uint32_t lastDebounceTime = 0;
uint32_t lastKeyPressTime = 0;
uint8_t lastKeyPressed = 0xFF;

uint32_t accumulatedNumber = 0;
uint8_t numberOfDigits = 0;


typedef enum {
    KEY_IDLE,
    KEY_DEBOUNCING,
    KEY_PRESSED,
    KEY_HOLDING
} KeyState;


typedef enum {
    SEQ_IDLE,
    SEQ_PRESSED_T,
    SEQ_PRESSED_T_L,
	SEQ_PRESSED_T_$,
	SEQ_PRESSED_T_F3,
	SEQ_PRESSED_T_F4,
	SEQ_NUMBER
} SequenceState;


KeyState keyState = KEY_IDLE;
SequenceState seqState = SEQ_IDLE;

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


void KeyLogic() {
	keyPressed = KeyPad_Scan();
	if (keyPressed != 0xFF) {
		if (seqState == SEQ_NUMBER) {
			if (keyPressed >= '0' && keyPressed <= '9') {
				if (numberOfDigits < 6) {
					accumulatedNumber = accumulatedNumber * 10 + (keyPressed - '0');
					numberOfDigits++;
				}
			} else {
				seqState = SEQ_IDLE;
				numberOfDigits = 0;
				accumulatedNumber = 0;
			}
		} else {
			switch (keyPressed) {
				case 'A':
					SevenSegBuffer[0] = 0;
					SevenSegBuffer[1] = 10000;
					SevenSegBuffer[2] = 0;
					break;
				case 'B':
					SevenSegBuffer[0] = 0;
					SevenSegBuffer[1] = 100000;
					SevenSegBuffer[2] = 0;
					break;
				case 'C':
					SevenSegBuffer[0] = 0;
					SevenSegBuffer[1] = 0;
					SevenSegBuffer[2] = 1;
					break;
				case 'E':
					SevenSegBuffer[0] = 0;
					SevenSegBuffer[1] = 0;
					SevenSegBuffer[2] = 100;
					break;
				case 'T':
					if (seqState == SEQ_IDLE) {
						seqState = SEQ_PRESSED_T;
					} else {
						seqState = SEQ_IDLE;
					}
					break;
				case '$':
					if (seqState == SEQ_PRESSED_T) {
						seqState = SEQ_PRESSED_T_$;
					} else {
						seqState = SEQ_IDLE;
					}
					break;
				case 'L':
					if (seqState == SEQ_PRESSED_T) {
						seqState = SEQ_PRESSED_T_L;
					} else {
						seqState = SEQ_IDLE;
					}
					break;
				case 'D':
					if (seqState == SEQ_PRESSED_T) {
						seqState = SEQ_PRESSED_T_F3;
					} else {
						seqState = SEQ_IDLE;
					}
					break;
				case 'F':
					if (seqState == SEQ_PRESSED_T) {
						seqState = SEQ_PRESSED_T_F4;
					} else {
						seqState = SEQ_IDLE;
					}
					break;
				default:
					if (keyPressed >= '0' && keyPressed <= '9') {
						seqState = SEQ_NUMBER;
						accumulatedNumber = keyPressed - '0';
						numberOfDigits = 1;
					} else {
						seqState = SEQ_IDLE;
					}
					break;
			}
		}
		keyPressed = 0xFF;
	}
}

void KeyLogic_Action() {
    switch (seqState) {
        case SEQ_IDLE:
            SevenSegBuffer[0] = 0;
            SevenSegBuffer[1] = 0;
            SevenSegBuffer[2] = 0;
            break;
        case SEQ_PRESSED_T:
            SevenSegBuffer[0] = 0;
            SevenSegBuffer[1] = 0;
            SevenSegBuffer[2] = 999999;
            break;
        case SEQ_PRESSED_T_$:
            SevenSegBuffer[0] = 0;
            SevenSegBuffer[1] = 0;
            SevenSegBuffer[2] = 111111;
            break;
        case SEQ_PRESSED_T_L:
            SevenSegBuffer[0] = 0;
            SevenSegBuffer[1] = 123456;
            SevenSegBuffer[2] = 0;
            break;
        case SEQ_PRESSED_T_F3:
            SevenSegBuffer[0] = 333333;
            SevenSegBuffer[1] = 0;
            SevenSegBuffer[2] = 0;
            break;
        case SEQ_PRESSED_T_F4:
            SevenSegBuffer[0] = 444444;
            SevenSegBuffer[1] = 0;
            SevenSegBuffer[2] = 0;
            break;
        case SEQ_NUMBER:
			SevenSegBuffer[0] = accumulatedNumber;
			SevenSegBuffer[1] = 0;
			SevenSegBuffer[2] = 0;
			break;
        default:
            SevenSegBuffer[0] = 0;
            SevenSegBuffer[1] = 0;
            SevenSegBuffer[2] = 0;
            break;
    }
}
