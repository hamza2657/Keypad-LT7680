/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LT768_Lib.h"
#include "math.h"
#include "stdint.h"
#include "touch_gtxxxx.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEBUG_ON		0
#define PASSWORD_LENGTH 4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern uint8_t PwmControl;
extern point_coor point;
uint16_t widgetDelay = 150;
char numToString[1],password[PASSWORD_LENGTH + 1];
uint8_t stored_password[PASSWORD_LENGTH+1] = {2,6,5,7} ;
uint16_t xStart, yStart, x_jump, y_jump;
uint8_t password_length  = 0;
uint16_t numx = 8, numy = 58, passwordStart,passRemoveRegion,clearDigitScreen_x1,clearDigitScreen_x2;
uint16_t rawDigitStart; //to store start x coordinate for displaying pass, and used in clearing screen to get
						//gain the exact start position.

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void drawKeyPad(int x , int y,uint8_t pwl);
void int_to_string(char *result, int num);
void Keypad_storePassword(uint8_t PW);
void remove_last_entry_from_password();
void check_Password();
void initiate_mainScreen();
void clear_password();
void updatewidget(uint8_t circleNum);

#if DEBUG_ON == 1
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

#endif
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */
  // 5 volt for the LCD to get ON
  HAL_GPIO_WritePin(GPIOA, LCD_5V_Pin, GPIO_PIN_SET);
  /*LCD initialization */
    LT768_Init();
  	Select_SFI_Dual_Mode0();
  	LT768_PWM0_Init(1,0,200,100,0);
  	LT768_PWM1_Init(1,0,200,100,PwmControl);
  	CTP_Init_Panel(); //touch panel
  	Display_ON(); //turn on display
  	LT768_setMainWindow();
  	//Main Screen Heading
  	LT768_DrawCircleSquare_Fill(158, 15, 590, 67,25, 25, MSColor);
  	LT768_DrawCircleSquare_Fill(513, 15, 641, 67,25, 25, MSIndicator);
  	LT768_DrawLeftUpCurve_Fill(540, 67, 60, 52, White);
  	LT768_DrawLeftUpCurve_Fill(540, 67, 48, 52, MSIndicator);
  	LT768_Select_Internal_Font_Init(32, 1, 1, 1, 1);
  	//Fonts Writing
  	LT768_Print_Internal_Font_String(187, 25, White, White, "INSTRUMENT AIR");
  	LT768_Select_Internal_Font_Init(24, 1, 1, 1, 1);
  	LT768_Print_Internal_Font_String(530, 30, White, White, "NORMAL");


  	/*
  	 * KeyPad Scanner
  	 * pass the starting x and y address
  	 */
  	drawKeyPad(317 , 150, PASSWORD_LENGTH);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint16_t x1 = 11,y1 = 14,x2 = 68,y2 = 72;
  while (1)
  {
	 if((point.input_x >= (xStart+x1) && point.input_y >= (yStart+y1) ) &&
			 ( point.input_x < (xStart+x2) && point.input_y < (yStart+y2)) )
			  {
				  point.input_x=0;
				  point.input_y=0;
				  updatewidget(1);
				  Keypad_storePassword(1);
			  }
		  //touch detection for 2
		  else if((point.input_x >= (xStart+x1+77) && point.input_y >= (yStart+y1) ) &&
				 ( point.input_x < (xStart+x2+77) && point.input_y < (yStart+y2)) )
				  {
					  point.input_x=0;
					  point.input_y=0;
					  updatewidget(2);
					  Keypad_storePassword(2);
					}
		  //touch detection for 3
		  else if((point.input_x >= (xStart+x1+154) && point.input_y >= (yStart+y1) ) &&
				 ( point.input_x < (xStart+x2+154) && point.input_y < (yStart+y2)) )
				  {
					  point.input_x=0;
					  point.input_y=0;
					  updatewidget(3);
					  Keypad_storePassword(3);
					}
		  //touch detection for 4
		  else if((point.input_x >= (xStart+x1) && point.input_y >= (yStart+y2+10) ) &&
				 ( point.input_x < (xStart+x2) && point.input_y < (yStart+y2+66)) )
				  {
					  point.input_x=0;
					  point.input_y=0;
					  updatewidget(4);
					  Keypad_storePassword(4);
					}
		  //touch detection for 5
		  else if((point.input_x >= (xStart+x1+77) && point.input_y >= (yStart+y2+10) ) &&
				 ( point.input_x < (xStart+x2+77) && point.input_y < (yStart+y2+66)) )
				  {
					  point.input_x=0;
					  point.input_y=0;
					  updatewidget(5);
					  Keypad_storePassword(5);
					}
		  //touch detection for 6
		  else if((point.input_x >= (xStart+x1+154) && point.input_y >= (yStart+y2+10) ) &&
				 ( point.input_x < (xStart+x2+154) && point.input_y < (yStart+y2+66)) )
				  {
					  point.input_x=0;
					  point.input_y=0;
					  updatewidget(6);
					  Keypad_storePassword(6);
					}
		  //touch detection for 7
		  else if((point.input_x >= (xStart+x1) && point.input_y >= (yStart+y2+76) ) &&
				 ( point.input_x < (xStart+x2) && point.input_y < (yStart+y2+132)) )
				  {
					  point.input_x=0;
					  point.input_y=0;
					  updatewidget(7);
					  Keypad_storePassword(7);
					}
		  //touch detection for 8
		  else if((point.input_x >= (xStart+x1+77) && point.input_y >= (yStart+y2+76) ) &&
				 ( point.input_x < (xStart+x2+77) && point.input_y < (yStart+y2+132)) )
				  {
					  point.input_x=0;
					  point.input_y=0;
					  updatewidget(8);
					  Keypad_storePassword(8);
					}
		  //touch detection for 9
		  else if((point.input_x >= (xStart+x1+154) && point.input_y >= (yStart+y2+76) ) &&
				 ( point.input_x < (xStart+x2+154) && point.input_y < (yStart+y2+132)) )
				  {
					  point.input_x=0;
					  point.input_y=0;
					  updatewidget(9);
					  Keypad_storePassword(9);
					}

		  //touch detection for 0
		  else if((point.input_x >= (xStart+x1+77) && point.input_y >= (yStart+y2+142) ) &&
				 ( point.input_x < (xStart+x2+77) && point.input_y < (yStart+y2+198)) )
				  {
					  point.input_x=0;
					  point.input_y=0;
					  updatewidget(11);  //11 indicating as 0
					  Keypad_storePassword(0);
					}


	 //touch detection for x
	  if((point.input_x >= (xStart+x1) && point.input_y >= (yStart+y2+142) ) &&
		 ( point.input_x < (xStart+x2) && point.input_y < (yStart+y2+198)) )
		  {
			  point.input_x=0;
			  point.input_y=0;
			  updatewidget(10);
			  remove_last_entry_from_password();
			}

	//touch detection for Enter
	  else if((point.input_x >= (xStart+x1+154) && point.input_y >= (yStart+y2+142) ) &&
			 ( point.input_x < (xStart+x2+154) && point.input_y < (yStart+y2+198)) )
			  {
				  point.input_x=0;
				  point.input_y=0;
				  updatewidget(12);
				  check_Password();

				}

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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void drawKeyPad(int x , int y,uint8_t pwl)
{
	xStart = x , yStart = y, x_jump = 40, y_jump = 43;
	int xEnd = x + 212 , yEnd = y + 284;
	uint8_t fillCount = 0,countnum = 1 ,count = 0, num = 1, checknum = 1;
	uint8_t line = pwl;  //for password entires
	/*
	 * updating password lines according to passward length
	 */
	if(0 == (line % 2))
		{
			//if password length is even
			uint8_t y1_2 = 22;
			uint16_t centerRec = xStart + 106;
			uint16_t x1 = 11, x2 = x1+32;
			line /= 2;
			uint8_t temp = line;
			while(line--)    //first half to the right from the center
			{
				LT768_DrawLine_Width((centerRec + x1), (yStart - y1_2) , (centerRec + x2) , (yStart - y1_2), White, 4);
				if(!line)	clearDigitScreen_x2 = (centerRec + x2);
				x1+=54;
				x2+=54;
			}
			x1 = centerRec-11, x2 = x1-32;
			while(temp--) //second half to the left from center
			{
				LT768_DrawLine_Width(x1, (yStart - y1_2) , x2 , (yStart - y1_2), White, 4);
				if(!temp)
					{
						clearDigitScreen_x1 = x2; //to remove the digits
						passwordStart = x2 ;
					}
				x1-=54;
				x2-=54;
			}

		}
	else
		{
			uint8_t y1_2 = 22;
			uint16_t centerRec = xStart + 106;
			uint16_t x1 = centerRec-16, x2 = centerRec+16;
			LT768_DrawLine_Width(x1, (yStart - y1_2) , x2 , (yStart - y1_2), White, 4); //centerd bar
			if(line>1)
			{
				line--;
				line /= 2;
				uint8_t temp = line;
				x1 -= 22;		//starting x-point of bars in the left direction from center
				x2 = (x1-32); //starting y-point of bars in the left direction	from center
				while(line--)
				{
					LT768_DrawLine_Width(x1, (yStart - y1_2) , x2 , (yStart - y1_2), White, 4);
					if(!(line))
						{
							clearDigitScreen_x1 = x2; //to remove the digits
							passwordStart = x2; // will be the start address of of first digit on the display
						}
					x1 -= 54;
					x2 -= 54;
				}
				x1 = ((centerRec+16)+22);  //starting x-point of bars in the right direction from center
				x2 = (x1 + 32);			//starting y-point of bars in the right direction from center
				while(temp--)
				{
					LT768_DrawLine_Width(x1, (yStart - y1_2) , x2 , (yStart - y1_2), White, 4);
					if(!temp)	clearDigitScreen_x2 =  x2;
					x1 += 54;
					x2 += 54;
				}

			}
			else
				{
					clearDigitScreen_x1 = x1; //to remove the digits
					clearDigitScreen_x2 = x2;
					passwordStart = x1; //in case if user chooses only 1 digit for password
				}

		}


	rawDigitStart = passwordStart; //to store the x-start point for displaying digits.
	/*
	 * Drawing keypad
	 */
	LT768_DrawCircleSquare_Width(xStart, yStart, xEnd, yEnd, 16, 16, White, BGcolor, 1);
	LT768_Select_Internal_Font_Init(32, 1, 1, 1, 1);
	while(count<12)
	{
		LT768_DrawCircle_Width(xStart + x_jump, yStart + y_jump , 29, White, BGcolor, 1);
		count++;
		if(!(countnum%4))
			{
				checknum += 1;
				num = checknum ;
				if(fillCount == 0)
				{
					LT768_DrawLine_Width((xStart + 34), (yStart + 250), (xStart + 46), (yStart + 237), White, 3);
					LT768_DrawLine_Width((xStart + 46), (yStart + 250), (xStart + 34), (yStart + 237), White, 3);
				}
				else if(fillCount == 1)
				{
					LT768_Print_Internal_Font_String(((xStart + x_jump) - 8), ((yStart + y_jump)-15), White, Black,"0");
				}
				else if(fillCount == 2)
				{
					//todo
				}
				fillCount++;
			}
		else
		{
			int_to_string(numToString, num);
			LT768_Print_Internal_Font_String(((xStart + x_jump) - 8), ((yStart + y_jump)-15), White, Black,numToString);
			num+=3;
		}
		y_jump += 67;
		countnum++;
		if(!(count%4))
			{
				x_jump += 67;
				y_jump = 43;
			}


	}

}

void int_to_string(char *result, int num)
{
  sprintf(result, "%d", num);
}
void Keypad_storePassword(uint8_t PW)
{
	if (password_length < PASSWORD_LENGTH)
	{
		LT768_Select_Internal_Font_Init(32, 1, 1, 1, 1);
		password[password_length] = PW;
		LT768_Print_Internal_Font_String(passwordStart+numx, yStart-numy, White, 0,"*");
		passRemoveRegion = ((passwordStart+numx) - 8);
		numx+=54;
		password_length++;
	}
	else if(password_length == (PASSWORD_LENGTH+1))
		{
			password_length = 0;
			numx = 8;
		}

}

void remove_last_entry_from_password()
{
	if (password_length > 0)
		{
			password_length--;
			password[password_length] = '\0'; // set last character to null terminator
			LT768_DrawSquare_Fill(passRemoveRegion, yStart-(numy+5), passRemoveRegion+32, yStart-27, BGcolor);
			passwordStart -= 54;
			passRemoveRegion -= 54;
		}
}

void check_Password()
{
	if (password_length == PASSWORD_LENGTH) { // we'll make sure password is fully entered
	    if (memcmp(password, stored_password, PASSWORD_LENGTH) == 0)
	    {
	      // password is correct
	      initiate_mainScreen();
	    }
	    else
	    {
	      clear_password();
	    }
	  }
}


void clear_password()
{
	for (int i = 0; i < PASSWORD_LENGTH; i++)
		{
	    	password[i] = '\0'; //fill all the spaces with null character
		}
	  password_length = 0;
	  LT768_DrawSquare_Fill(clearDigitScreen_x1, yStart-(numy+5), clearDigitScreen_x2, yStart-27, BGcolor);
	  passwordStart = rawDigitStart;
	  numx = 8; //starting distance of digit from the start of line

}
void initiate_mainScreen()
{
	LT768_DrawSquare_Fill(0, 0, 800, 480, White);
	LT768_Select_Internal_Font_Init(32, 2, 2, 1, 1);
	LT768_Print_Internal_Font_String(300, 220, Black, 0, "We are working on it! have patience!");
	while(1);

}
void updatewidget(uint8_t circleNum)
{
	LT768_Select_Internal_Font_Init(32, 1,1, 1, 1);
	int_to_string(numToString, circleNum);
	if(circleNum == 1 || circleNum == 2 || circleNum == 3 )
		{
			LT768_DrawCircle_Width(xStart + (40 * circleNum) + (27 *(circleNum-1)), (yStart + 43) , 29, White, DarkGrey, 1);
			LT768_Print_Internal_Font_String((xStart + (40 * circleNum) + (27 *(circleNum-1)))-8, yStart+28, White, 0, numToString);
			HAL_Delay(widgetDelay);
			LT768_DrawCircle_Width(xStart + (40 * circleNum) + (27 *(circleNum-1)), (yStart + 43) , 29, White, BGcolor, 1);
			LT768_Print_Internal_Font_String((xStart + (40 * circleNum) + (27 *(circleNum-1)))-8, yStart+28, White, 0, numToString);
		}
	else if(circleNum == 4 || circleNum == 5 || circleNum == 6 )
		{
			LT768_DrawCircle_Width(xStart + (40 * (circleNum-3)) + (27 *((circleNum-4))),(yStart + 110)  , 29, White, DarkGrey, 1);
			LT768_Print_Internal_Font_String((xStart + (40 * (circleNum-3)) + (27 *((circleNum-4))))-8, yStart+95, White, 0, numToString);
			HAL_Delay(widgetDelay);
			LT768_DrawCircle_Width(xStart + (40 * (circleNum-3)) + (27 *((circleNum-4))), (yStart + 110) , 29, White, BGcolor, 1);
			LT768_Print_Internal_Font_String((xStart + (40 * (circleNum-3)) + (27 *((circleNum-4))))-8, yStart+95, White, 0, numToString);
		}
	else if(circleNum == 7 || circleNum == 8 || circleNum == 9 )
		{
			LT768_DrawCircle_Width(xStart + (40 * (circleNum-6)) + (27 *((circleNum-7))),(yStart + 177)  , 29, White, DarkGrey, 1);
			LT768_Print_Internal_Font_String((xStart + (40 * (circleNum-6)) + (27 *((circleNum-7))))-8, yStart+162, White, 0, numToString);
			HAL_Delay(widgetDelay);
			LT768_DrawCircle_Width(xStart + (40 * (circleNum-6)) + (27 *((circleNum-7))), (yStart + 177) , 29, White, BGcolor, 1);
			LT768_Print_Internal_Font_String((xStart + (40 * (circleNum-6)) + (27 *((circleNum-7))))-8, yStart+162, White, 0, numToString);
		}
	else if(circleNum == 10 || circleNum == 11 || circleNum == 12 )
			{
				if(circleNum == 11)
				{
					LT768_DrawCircle_Width(xStart + (40 * (circleNum-9)) + (27 *((circleNum-10))),(yStart + 244)  , 29, White, DarkGrey, 1);
					LT768_Print_Internal_Font_String((xStart + (40 * (circleNum-9)) + (27 *((circleNum-10))))-8, yStart+229, White, 0, "0");
					HAL_Delay(widgetDelay);
					LT768_DrawCircle_Width(xStart + (40 * (circleNum-9)) + (27 *((circleNum-10))), (yStart + 244) , 29, White, BGcolor, 1);
					LT768_Print_Internal_Font_String((xStart + (40 * (circleNum-9)) + (27 *((circleNum-10))))-8, yStart+229, White, 0, "0");
				}
				else if(circleNum == 10)
				{
					LT768_DrawCircle_Width(xStart + (40 * (circleNum-9)) + (27 *((circleNum-10))),(yStart + 244)  , 29, White, DarkGrey, 1);
					LT768_DrawLine_Width((xStart + 34), (yStart + 250), (xStart + 46), (yStart + 237), White, 3);
					LT768_DrawLine_Width((xStart + 46), (yStart + 250), (xStart + 34), (yStart + 237), White, 3);
					HAL_Delay(widgetDelay);
					LT768_DrawCircle_Width(xStart + (40 * (circleNum-9)) + (27 *((circleNum-10))), (yStart + 244) , 29, White, BGcolor, 1);
					LT768_DrawLine_Width((xStart + 34), (yStart + 250), (xStart + 46), (yStart + 237), White, 3);
					LT768_DrawLine_Width((xStart + 46), (yStart + 250), (xStart + 34), (yStart + 237), White, 3);
				}
				else
				{
					LT768_DrawCircle_Width(xStart + (40 * (circleNum-9)) + (27 *((circleNum-10))),(yStart + 244)  , 29, White, DarkGrey, 1);
					HAL_Delay(widgetDelay);
					LT768_DrawCircle_Width(xStart + (40 * (circleNum-9)) + (27 *((circleNum-10))), (yStart + 244) , 29, White, BGcolor, 1);
				}
			}

}
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
