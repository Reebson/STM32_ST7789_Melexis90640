/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "rng.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "st7789.h"
#include "MLX90640.h"
#include "MLX90640_API.h"
#include "img.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TA_SHIFT 8
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

float mlx90640To[768];
uint16_t eeMLX90640[832];
paramsMLX90640 mlx90640;
uint16_t mlx90640Frame[834];
int status = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void drawImage() {
	int i, j;
	int x,y,w,h,pixel_size;

	if(getInterpolate())
		pixel_size=4;				//domyslnie pixel_size=6; 4
	else pixel_size=PX_SIZE;

	w=IMG_X_SIZE/6;					//IMG_X_SIZE/pixel_size; 6
	h=IMG_Y_SIZE/6;

	x=0;
	y=0;

	for(i=0; i<h; i++) {
		for(j=0; j<w; j++) {
			ST7789_DrawPixel_4px(x+pixel_size, y+pixel_size, getPixelColor(getImagePixel(j,i)));
		  	x+=(pixel_size);
		}
		y+=(pixel_size);
		x=0;
	}
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &hspi2)
	{
		//HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
		lcd_transfer_done();
	}
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
  MX_DMA_Init();
  MX_SPI2_Init();
  MX_RNG_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  ST7789_Init();
  Init_MLX90640_GPIO(hi2c1);

  hi2c1.Init.Timing = 0x00501E63; //FM !!! must be change if clk is changed
  HAL_I2C_Init(&hi2c1);

  MLX90640_SetRefreshRate(MLX90640_RATE_16HZ);
  MLX90640_SetResolution(MLX90640_RES18);
  MLX90640_SetPattern(MLX90640_CHESS);
  MLX90640_SetMode(MLX90640_DEFAULT);

  MLX90640_DumpEE(eeMLX90640);
  status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
  HAL_Delay(10);
  setColorMode(1);

  ST7789_Fill_Color(WHITE);

  uint32_t legendColor=0;
  int a;

  for(a=0; a<255; a++)
  {
	 legendColor=getPixelColor(255-a);
	 for(int j = 0; j < 6; j++) {
		ST7789_DrawPixel_4px(230 - a, 225 + j, legendColor);
	 }
  }


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  char buff[20];
	  int value = 3;

	  status = MLX90640_GetFrameData(mlx90640Frame);

	  float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);
	  float tr = Ta - TA_SHIFT; //Reflected temperature based on the sensor ambient temperature
	  float emissivity = 0.95;

	  MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);
	  MLX90640_BadPixelsCorrection((&mlx90640)->brokenPixels, mlx90640To, 0, &mlx90640);
	  MLX90640_BadPixelsCorrection((&mlx90640)->outlierPixels, mlx90640To, 0, &mlx90640);
	  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);

	  int i,j,k=0;
	  float varMaxTempVal=0.0, varMinTempVal=300;
	  for(i=0; i<24; i++)
	  {
		  for(j=0; j<32; j++)
		  {
			  float tmp = mlx90640To[k];
			  if(tmp > varMaxTempVal)
				  varMaxTempVal = (tmp); //find max Temp pixel
			  if(tmp < varMinTempVal)
				  varMinTempVal = (tmp); //find maxTemp pixel
			  setImagePixel(j,i,tmp);
			  k++;
		  }
	  }

//	  if(getAutoscale())
//	  {
//		  if(varMaxTempVal==varMinTempVal)varMaxTempVal+=1; // to avoid zero division
//		  setMaxTemp(varMaxTempVal);
//		  setMinTemp(varMinTempVal);
//	  }

	  if(getInterpolate()) {
		  interpolateBilinear(PX_X*INT_POINTS, PX_Y*INT_POINTS);
	  }
	  //while(lcd_is_busy()) {}
	  drawImage();

		//display max temp value
	  ST7789_WriteString(200, 210, gcvt(varMaxTempVal, value, buff), Font_7x10, WHITE, BLACK);
		//display min temp value
	  ST7789_WriteString(10, 210, gcvt(varMinTempVal, value, buff), Font_7x10, WHITE, BLACK);

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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
