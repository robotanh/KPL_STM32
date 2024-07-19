#include "KeyPad.h"


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

uint32_t row1 =0; 	//Display total liters
uint32_t row2 =0;	//Display total liters

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
// T + L pressed show total from beginning
void formatTotalLiters(long unsigned int total, uint32_t* buffer1, uint32_t* buffer2)
{
	if (total < 100000000) {
		* buffer1 = total / 1000000;
		* buffer2 = total % 1000000;
		LEDPointFlag = 3;

	} else {
		* buffer1 = total / 100000000;
		* buffer2 = (total % 100000000 ) /100;
		LEDPointFlag = 1;
	}
}
// T + $ pressed show total per shift
void formatTotalLitersShift(long unsigned int total, uint32_t* buffer1, uint32_t* buffer2)
{
	if (total < 1000000000) {

		* buffer1 = total / 1000000;
		* buffer2 = total % 1000000;
		LEDPointFlag = 3;

	} else {
		* buffer1 =0;
		* buffer2 = 0;
		LEDPointFlag = -11;
	}
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
                    snprintf(SevenSegBuffer[0], sizeof(SevenSegBuffer[0]), "%06d", 0);
                    snprintf(SevenSegBuffer[1], sizeof(SevenSegBuffer[1]), "%06d", 10000);
                    snprintf(SevenSegBuffer[2], sizeof(SevenSegBuffer[2]), "%06d", 0);
                    break;
                case 'B':
                    snprintf(SevenSegBuffer[0], sizeof(SevenSegBuffer[0]), "%06d", 0);
                    snprintf(SevenSegBuffer[1], sizeof(SevenSegBuffer[1]), "%06d", 100000);
                    snprintf(SevenSegBuffer[2], sizeof(SevenSegBuffer[2]), "%06d", 0);
                    break;
                case 'C':
                    snprintf(SevenSegBuffer[0], sizeof(SevenSegBuffer[0]), "%06d", 0);
                    snprintf(SevenSegBuffer[1], sizeof(SevenSegBuffer[1]), "%06d", 0);
                    snprintf(SevenSegBuffer[2], sizeof(SevenSegBuffer[2]), "%06d", 1);
                    break;
                case 'E':
                    snprintf(SevenSegBuffer[0], sizeof(SevenSegBuffer[0]), "%06d", 0);
                    snprintf(SevenSegBuffer[1], sizeof(SevenSegBuffer[1]), "%06d", 0);
                    snprintf(SevenSegBuffer[2], sizeof(SevenSegBuffer[2]), "%06d", 100);
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
    char buffer[7];
    switch (seqState) {
        case SEQ_IDLE:
            snprintf(SevenSegBuffer[0], sizeof(SevenSegBuffer[0]), "%06d", 0);
            snprintf(SevenSegBuffer[1], sizeof(SevenSegBuffer[1]), "%06d", 0);
            snprintf(SevenSegBuffer[2], sizeof(SevenSegBuffer[2]), "%06d", 0);
            break;
        case SEQ_PRESSED_T:
            snprintf(SevenSegBuffer[0], sizeof(SevenSegBuffer[0]), "%06d", 0);
            snprintf(SevenSegBuffer[1], sizeof(SevenSegBuffer[1]), "%06d", 0);
            snprintf(SevenSegBuffer[2], sizeof(SevenSegBuffer[2]), "%06d", 999999);
            break;
        case SEQ_PRESSED_T_$:
        	// Format the total liters into two parts
			formatTotalLitersShift(totalLitersShift, &row1, &row2);

			// Ensure the combined string fits into the buffer
			char row1StrShift[7]; // Buffer to hold formatted row1 string
			snprintf(row1StrShift, sizeof(row1StrShift), "%06ld", row1);

			// Combine "L.. " with the last two digits of row1
			char combinedStrShift[8]; // Buffer to hold combined string "L.. " and last two digits of row1
			snprintf(combinedStrShift, sizeof(combinedStrShift), "SH.%04ld", row1 % 10000); // Extract last two digits of row1

			// Fill SevenSegBuffer[0] with combinedStr and pad with spaces if necessary
			for (int i = 0; i < 6; ++i) {
				if (i < strlen(combinedStrShift)) {
					SevenSegBuffer[0][i] = combinedStrShift[i];
				} else {
					SevenSegBuffer[0][i] = ' '; // Pad with spaces
				}
			}
			snprintf(SevenSegBuffer[1], sizeof(SevenSegBuffer[1]), "%06ld", row2);
			snprintf(SevenSegBuffer[2], sizeof(SevenSegBuffer[2]), "SHIFT ");

            break;
        case SEQ_PRESSED_T_L:
            // Format the total liters into two parts
            formatTotalLiters(totalLiters, &row1, &row2);

            // Ensure the combined string fits into the buffer
            char row1Str[7]; // Buffer to hold formatted row1 string
            snprintf(row1Str, sizeof(row1Str), "%06ld", row1);

            // Combine "L.. " with the last two digits of row1
            char combinedStr[8]; // Buffer to hold combined string "L.. " and last two digits of row1
            snprintf(combinedStr, sizeof(combinedStr), "L.%04ld", row1 % 10000); // Extract last two digits of row1

            // Fill SevenSegBuffer[0] with combinedStr and pad with spaces if necessary
            for (int i = 0; i < 6; ++i) {
                if (i < strlen(combinedStr)) {
                    SevenSegBuffer[0][i] = combinedStr[i];
                } else {
                    SevenSegBuffer[0][i] = ' '; // Pad with spaces
                }
            }


            snprintf(SevenSegBuffer[1], sizeof(SevenSegBuffer[1]), "%06ld", row2);
            snprintf(SevenSegBuffer[2], sizeof(SevenSegBuffer[2]), "TOTAL ");


            break;
        case SEQ_PRESSED_T_F3:
            snprintf(SevenSegBuffer[0], sizeof(SevenSegBuffer[0]), "%06d", 333333);
            snprintf(SevenSegBuffer[1], sizeof(SevenSegBuffer[1]), "%06d", 0);
            snprintf(SevenSegBuffer[2], sizeof(SevenSegBuffer[2]), "%06d", 0);
            break;
        case SEQ_PRESSED_T_F4:
            snprintf(SevenSegBuffer[0], sizeof(SevenSegBuffer[0]), "%06d", 444444);
            snprintf(SevenSegBuffer[1], sizeof(SevenSegBuffer[1]), "%06d", 0);
            snprintf(SevenSegBuffer[2], sizeof(SevenSegBuffer[2]), "%06d", 0);
            break;
        case SEQ_NUMBER:
            snprintf(buffer, sizeof(buffer), "%06ld", accumulatedNumber);
            snprintf(SevenSegBuffer[0], sizeof(SevenSegBuffer[0]), "%s", buffer);
            snprintf(SevenSegBuffer[1], sizeof(SevenSegBuffer[1]), "%06d", 0);
            snprintf(SevenSegBuffer[2], sizeof(SevenSegBuffer[2]), "%06d", 0);
            LEDPointFlag = -1;
            break;
        default:
            snprintf(SevenSegBuffer[0], sizeof(SevenSegBuffer[0]), "%06d", 0);
            snprintf(SevenSegBuffer[1], sizeof(SevenSegBuffer[1]), "%06d", 0);
            snprintf(SevenSegBuffer[2], sizeof(SevenSegBuffer[2]), "%06d", 0);
            break;
    }
}
