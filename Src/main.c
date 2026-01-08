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
#include "adc.h"
#include "can.h"
#include "crc.h"
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "rng.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "button.h"
#include "buzzer.h"
#include "can_if.h"
#include "timer.h"
#include "BMI088driver.h"
#include "reference.h"
#include "Mahony.h"
#include "WS281x.h"
#include "DR16_Remote.h"
#include "event_os.h"
#include "Robo_Common.h"
#include "Robo_Chassis.h"
#include "Robo_Control.h"
#include "Robo_gimbal.h"
#include "Robo_Shoot.h"
#include "Robo_USB.h"
#include "Robo_AA.h"
#include "RC_Ctrl_Key.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// #define Set_LED_R(status) HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, status)
// #define Set_LED_G(status) HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, status)
// #define Set_LED_B(status) HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, status)
// #define Toggle_LED_R() HAL_GPIO_TogglePin(LEDR_GPIO_Port, LEDR_Pin)
// #define Toggle_LED_G() HAL_GPIO_TogglePin(LEDG_GPIO_Port, LEDG_Pin)
// #define Toggle_LED_B() HAL_GPIO_TogglePin(LEDB_GPIO_Port, LEDB_Pin)
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
Soft_Timer_t button_timer, timer_500ms, timer_init;
GPIO_Handle_t key_gpio;
Button_t key;
Button_t key1, key2;
Keyboard_t keyboard;
BMI088_Data_t BMI088;
extern Task_Chassis_t chassis;
/* pwm 占空比数值为uint16_t 类型，DMA传输时只能以半字输出，pixelBuffer应为uint16_t 类型 */
Buzzer_t buzzer;
uint32_t* referee;
uint16_t* power_limit_point;
Reference_t reference;
WS281x_PWM_Driver_t ws281x_pwm_driver;
WS281x_t *ws281x;
uint16_t ws281x_buffer[1024*GRB];
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
// void led_G_flash(void *config)
// {
// 	Toggle_LED_G();
// }
// void led_R_flash(void *config)
// {
// 	Toggle_LED_R();
// }
// void led_B_flash(void *config)
// {
// 	Toggle_LED_B();
// }
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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_SPI1_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_USART3_UART_Init();
  MX_RNG_Init();
  MX_RTC_Init();
  MX_TIM1_Init();
  MX_TIM10_Init();
  MX_USART1_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM8_Init();
  MX_I2C2_Init();
  MX_I2C3_Init();
  MX_SPI2_Init();
  MX_CRC_Init();
  MX_DAC_Init();
  MX_USB_DEVICE_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
	// 初始化按键
	//SEML_GPIO_Pin_Register(&key.GPIO_Handle, KEY_GPIO_Port, KEY_Pin);
	//Button_Init(&key, RESET);
	// 初始化定时器
	HAL_TIM_RegisterCallback(&htim6, HAL_TIM_PERIOD_ELAPSED_CB_ID, SEML_Timer_Callback);
	HAL_TIM_Base_Start_IT(&htim6);
	// 初始化imu
	SEML_GPIO_Pin_Register(&BMI088.CS1_Accel_handle,CS1_ACCEL_GPIO_Port,CS1_ACCEL_Pin);
	SEML_GPIO_Pin_Register(&BMI088.CS1_Gyro_handle,CS1_GYRO_GPIO_Port,CS1_GYRO_Pin);
	Hardware_SPI_Register(&BMI088.SPI_Handle,&hspi1);
	while(BMI088_Init(&BMI088));
	// 初始化中间件
	Robo_Middleware_Init();
	SEML_Timer_Delete(&timer_init);
	Buzzer_Init(&buzzer,&htim4,TIM_CHANNEL_3,84000000);
	Buzzer_Task_B_B_B(&buzzer,100);
	// 初始化DBUS
	DR16_Init(&RC_Ctrl,&huart3);
	// 初始化键盘
	Key_Init(&keyboard);
	Key_Register(&keyboard);
	// 初始化事件操作系统
	HAL_TIM_RegisterCallback(&htim7, HAL_TIM_PERIOD_ELAPSED_CB_ID, Event_OS_Callback);
	static Event_Task_Handle_t control_task_handle,chassis_task_handle,gambal_task_handle,shoot_task_handle,USB_task_handle,AA_task_handle;
	Event_OS_Task_Register(&control_task_handle,1,EOS_High_Priority,Robo_Control_Init,Robo_Control_Task,NULL);
	Event_OS_Task_Register(&chassis_task_handle,1,EOS_High_Priority,Chassis_Init,Chassis_Task,NULL);
	Event_OS_Task_Register(&gambal_task_handle,1,EOS_High_Priority,Gimbal_Init,Gimbal_Task,NULL);
  Event_OS_Task_Register(&shoot_task_handle,1,EOS_High_Priority,Shoot_Init,Shoot_Task,NULL);
  Event_OS_Task_Register(&USB_task_handle,1,EOS_High_Priority,USB_Init,USB_Task,NULL);
  Event_OS_Task_Register(&AA_task_handle,1,EOS_High_Priority,AA_Init,AA_Task,NULL);
	HAL_TIM_Base_Start_IT(&htim7);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	// 启动定时器
	SEML_Timer_Start(&timer_500ms);
	Buzzer_Task_B_B_B(&buzzer,100);
	// 启动事件操作系统
	Event_OS_Run();
  while (1)
  {
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
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
