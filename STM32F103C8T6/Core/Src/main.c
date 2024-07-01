/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "LED_Screen.h"
#include "KeyPad.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

osThreadId SevenSegsHandle;
osThreadId KeyBoardHandle;
osThreadId LCDHandle;
/* USER CODE BEGIN PV */
uint8_t keyPressed = 0xFF;
uint32_t lcd_num = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);
static void MX_SPI1_Init(void);
void SevenSegs_Handler(void const * argument);
void KeyBoard_Handler(void const * argument);
void LCD_Handler(void const * argument);

/* USER CODE BEGIN PFP */
void ShiftOut(uint8_t data);
void ShiftOut_SPI(uint8_t *data, size_t size);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void ShiftOut(uint8_t data)
{
	uint8_t temp;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	for(int i=0;i<8;i++){
		temp = data & (0x80 >> i);
		if(temp == 0) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
		else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	}
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
}
void ShiftOut_SPI(uint8_t *data, size_t size)
{

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); // Pull STCP (Latch) low
    HAL_SPI_Transmit(&hspi2, data, size, 300); // Transmit data
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); // Pull STCP (Latch) high

}

void ShiftOut_LCD(uint8_t *data, size_t size)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); // Pull STCP (Latch) low
    HAL_SPI_Transmit(&hspi1, data, size, 300); // Transmit data
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); // Pull STCP (Latch) high
}

uint8_t* Num_Buff_Conv(uint32_t num) {
    // Ensure the number is within the valid range
    if (num > 99999999) {
        return NULL; // Return NULL for invalid input
    }

    static uint8_t output[8]; // Static array to hold the result
    for (int i = 0; i < 8; i++) {
        output[i] = digitMapWithDP[num % 10]; // Get the least significant digit
        num /= 10;            // Remove the least significant digit from the number
    }

    return output;
}

void Update_LCD(uint32_t num){
	uint8_t* buffer=Num_Buff_Conv(num);
	if(buffer==NULL){
		return;
	}
	ShiftOut_LCD(buffer,8);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI2_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */


  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of SevenSegs */
  osThreadDef(SevenSegs, SevenSegs_Handler, osPriorityNormal, 0, 128);
  SevenSegsHandle = osThreadCreate(osThread(SevenSegs), NULL);

  /* definition and creation of KeyBoard */
  osThreadDef(KeyBoard, KeyBoard_Handler, osPriorityHigh, 0, 128);
  KeyBoardHandle = osThreadCreate(osThread(KeyBoard), NULL);

  /* definition and creation of LCD */
  osThreadDef(LCD, LCD_Handler, osPriorityAboveNormal, 0, 128);
  LCDHandle = osThreadCreate(osThread(LCD), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1) {
//	  if(timer_flag[0]==1){
//		  uint8_t led_buffer[] = {0b11111111,digitMapWithDP[i%10],digitMapWithDP[(i+1)%10],digitMapWithDP[(i+2)%10]}; // Data to display '1' with DP
//		  ShiftOut_SPI(led_buffer, 4);
//		  i++;
//		  setTimer(0,100);
//	  }
//	  if(timer_flag[1]==1){
//		  keyPressed = KeyPad_Scan();
//		  if (keyPressed != 0xFF) // If a key is pressed
//		  {
//			  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
//
//		  }
//		  setTimer(1,100);
//	  }
//	  if(timer_flag[2]==1){
//
//		  Update_LCD(lcd_num);
//		  lcd_num*=2;
//		  setTimer(2,100);
//	  }


//	  ShiftOut(digitMapWithDP[0]);
//	  HAL_Delay(1000);
//	  ShiftOut(digitMapWithDP[1]);
//	  HAL_Delay(1000);
//	  ShiftOut(digitMapWithDP[2]);
//	  HAL_Delay(1000);
//	  ShiftOut(digitMapWithDP[3]);
//	  HAL_Delay(1000);
//	  ShiftOut(digitMapWithDP[4]);
//	  HAL_Delay(1000);
//	  ShiftOut(digitMapWithDP[5]);
//	  HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_Pin|Latch_SPI1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Latch_SPI_Pin|OUT0_Pin|OUT1_Pin|OUT2_Pin
                          |OUT3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_Pin Latch_SPI1_Pin */
  GPIO_InitStruct.Pin = LED_Pin|Latch_SPI1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : D0_Pin D1_Pin D2_Pin D3_Pin
                           D4_Pin */
  GPIO_InitStruct.Pin = D0_Pin|D1_Pin|D2_Pin|D3_Pin
                          |D4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : Latch_SPI_Pin */
  GPIO_InitStruct.Pin = Latch_SPI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(Latch_SPI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : OUT0_Pin OUT1_Pin OUT2_Pin OUT3_Pin */
  GPIO_InitStruct.Pin = OUT0_Pin|OUT1_Pin|OUT2_Pin|OUT3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/* USER CODE BEGIN Header_SevenSegs_Handler */
/**
  * @brief  Function implementing the SevenSegs thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_SevenSegs_Handler */
void SevenSegs_Handler(void const * argument)
{
  /* USER CODE BEGIN 5 */
	int i=0;
  /* Infinite loop */
  for(;;)
  {
	uint8_t led_buffer[] = {0b11111111,digitMapWithDP[i%10],digitMapWithDP[(i+1)%10],digitMapWithDP[(i+2)%10]}; // Data to display '1' with DP
	ShiftOut_SPI(led_buffer, 4);
	i++;
	osDelay(1000);
  }

  // In case we accidentally exit from task loop
  osThreadTerminate(NULL);

  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_KeyBoard_Handler */
/**
* @brief Function implementing the KeyBoard thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_KeyBoard_Handler */
void KeyBoard_Handler(void const * argument)
{
  /* USER CODE BEGIN KeyBoard_Handler */
  /* Infinite loop */
  for(;;)
  {
	keyPressed = KeyPad_Scan();
	if (keyPressed != 0xFF) // If a key is pressed
	{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);

	}
    osDelay(100);
  }

  // In case we accidentally exit from task loop
  osThreadTerminate(NULL);

  /* USER CODE END KeyBoard_Handler */
}

/* USER CODE BEGIN Header_LCD_Handler */
/**
* @brief Function implementing the LCD thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LCD_Handler */
void LCD_Handler(void const * argument)
{
  /* USER CODE BEGIN LCD_Handler */
  /* Infinite loop */
  for(;;)
  {
    Update_LCD(lcd_num);
    lcd_num*=2;
    osDelay(1);
  }

  // In case we accidentally exit from task loop
  osThreadTerminate(NULL);

  /* USER CODE END LCD_Handler */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM3 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM3) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
