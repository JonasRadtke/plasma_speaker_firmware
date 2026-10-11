/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tusb.h"
#include "oszillatoren.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "bandpass.h"

#ifdef __cplusplus
}
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
extern chan channel[8];
extern TIM_HandleTypeDef htim10;
extern TIM_HandleTypeDef htim11;
extern uint32_t arrRight;
extern uint32_t arrLeft;
extern UART_HandleTypeDef huart1;
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim3;
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */
		uint32_t burstMode = 1;
		int64_t outValueRight = 0;
		int64_t outValueLeft = 0;

		for(uint32_t i = 0; i < NO_OSCI; i++){
			oscillator(&channel[i]); // Oscillator Channel
		}

		outValueRight += (channel[0].chan_out); // Accumulate Oscillator
		outValueRight += (channel[2].chan_out); // Accumulate Oscillator
		outValueRight += (channel[4].chan_out); // Accumulate Oscillator
		outValueRight += (channel[6].chan_out); // Accumulate Oscillator
		outValueLeft += (channel[1].chan_out); // Accumulate Oscillator
		outValueLeft += (channel[3].chan_out); // Accumulate Oscillator
		outValueLeft += (channel[5].chan_out); // Accumulate Oscillator
		outValueLeft += (channel[7].chan_out); // Accumulate Oscillator
		// Clipping absichern, da ohne Division mehrere Kanäle sich aufsummieren können

		if(outValueRight > 0x1FFFFFFF){
			uint8_t outvalue[4];
			outvalue[0] = 0xFF;
			HAL_UART_Transmit(&huart1, outvalue, 4, HAL_MAX_DELAY);
		}


		if(outValueRight < -0x1FFFFFFF){
			uint8_t outvalue[4];
			outvalue[0] = 0xAA;
			HAL_UART_Transmit(&huart1, outvalue, 4, HAL_MAX_DELAY);
		}

		if (outValueRight > 0x3FFFFFFF) outValueRight = 0x3FFFFFFF;
		if (outValueRight < -0x3FFFFFFF) outValueRight = -0x3FFFFFFF;
		if (outValueLeft > 0x3FFFFFFF) outValueLeft = 0x3FFFFFFF;
		if (outValueLeft < -0x3FFFFFFF) outValueLeft = -0x3FFFFFFF;

		int32_t clampedRight = (int32_t)outValueRight;//process_sample((int32_t)outValueRight);
		int32_t clampedLeft = (int32_t)outValueRight;//process_sample((int32_t)outValueLeft);

		if(burstMode == 1){
			static bool burstRunningRight = false;
			static bool burstRunningLeft = false;
			if(clampedRight > 0){
				if (!burstRunningRight) {
					htim10.Instance->CCR1 = arrRight/2;
					burstRunningRight = true;
				}
			}
			if(clampedRight <= 0){
				if (burstRunningRight) {
					htim10.Instance->ARR = arrRight;
					htim10.Instance->CCR1 = 0;   // Duty Cycle auf 0% setzen
					burstRunningRight = false;
				}
			}
			if(clampedLeft > 0){
				if (!burstRunningLeft) {
					htim11.Instance->CCR1 = arrLeft/2;
					burstRunningLeft = true;
				}
			}
			if(clampedLeft <= 0){
				if (burstRunningLeft) {
					htim11.Instance->ARR = arrLeft;
					htim11.Instance->CCR1 = 0;   // Duty Cycle auf 0% setzen
					burstRunningLeft = false;
				}
			}
		}
		else if(burstMode == 2){
			// 10 Bit PWM
			// CW-Modus: 70-90% Duty Cycle relativ zum aktuellen ARR (freq)
			htim10.Instance->ARR = arrRight;
			htim11.Instance->ARR = arrLeft;

			const int32_t ccrMinRight = (arrRight * 30) / 100;
			const int32_t ccrMaxRight = (arrRight * 55) / 100;
			const int32_t ccrMidRight = (ccrMinRight + ccrMaxRight) / 2;
			const int32_t ccrRangeRight = (ccrMaxRight - ccrMinRight) / 2;
			const int32_t limitRight = 0x3FFFFFFF;

			const int32_t ccrMinLeft = (arrLeft * 30) / 100;
			const int32_t ccrMaxLeft = (arrLeft * 55) / 100;
			const int32_t ccrMidLeft = (ccrMinLeft + ccrMaxLeft) / 2;
			const int32_t ccrRangeLeft = (ccrMaxLeft - ccrMinLeft) / 2;
			const int32_t limitLeft = 0x3FFFFFFF;

			int32_t pwmvalueRight = ccrMidRight + (int32_t)(((int64_t)clampedRight * ccrRangeRight) / limitRight);
			int32_t pwmvalueLeft = ccrMidLeft + (int32_t)(((int64_t)clampedLeft * ccrRangeLeft) / limitLeft);
			htim10.Instance->CCR1 = (uint16_t)pwmvalueRight;
			htim11.Instance->CCR1 = (uint16_t)pwmvalueLeft;
		}
		else {
			if(channel[0].oscillator_on || channel[2].oscillator_on || channel[4].oscillator_on|| channel[6].oscillator_on){
				htim10.Instance->ARR = arrRight;
				const int32_t ccrMinRight = (arrRight * 30) / 100;
				const int32_t ccrMaxRight = (arrRight * 55) / 100;
				const int32_t ccrMidRight = (ccrMinRight + ccrMaxRight) / 2;
				const int32_t ccrRangeRight = (ccrMaxRight - ccrMinRight) / 2;
				const int32_t limitRight = 0x3FFFFFFF;

				int32_t pwmvalueRight = ccrMidRight + (int32_t)(((int64_t)clampedRight * ccrRangeRight) / limitRight);
				htim10.Instance->CCR1 = (uint16_t)pwmvalueRight;
			}
			else{
				htim10.Instance->ARR = arrRight;
				htim10.Instance->CCR1 = 0;   // Duty Cycle auf 0% setzen
			}
			if(channel[1].oscillator_on || channel[3].oscillator_on || channel[5].oscillator_on|| channel[7].oscillator_on){
				htim11.Instance->ARR = arrLeft;
				const int32_t ccrMinLeft = (arrLeft * 30) / 100;
				const int32_t ccrMaxLeft = (arrLeft * 55) / 100;
				const int32_t ccrMidLeft = (ccrMinLeft + ccrMaxLeft) / 2;
				const int32_t ccrRangeLeft = (ccrMaxLeft - ccrMinLeft) / 2;
				const int32_t limitLeft = 0x3FFFFFFF;

				int32_t pwmvalueLeft = ccrMidLeft + (int32_t)(((int64_t)clampedLeft * ccrRangeLeft) / limitLeft);
				htim11.Instance->CCR1 = (uint16_t)pwmvalueLeft;
			}
			else{
				htim11.Instance->ARR = arrLeft;
				htim11.Instance->CCR1 = 0;   // Duty Cycle auf 0% setzen
			}
		}

		 __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);
		 return;
  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */
  __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);
  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles USB On The Go FS global interrupt.
  */
void OTG_FS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_FS_IRQn 0 */
	tusb_int_handler(0, true);
	return;
  /* USER CODE END OTG_FS_IRQn 0 */
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
  /* USER CODE BEGIN OTG_FS_IRQn 1 */

  /* USER CODE END OTG_FS_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
