#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "smg.h"

#define loop for(;;)

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim1;

/* ADC DMA 缓冲区（仅一路） */
static uint16_t adc_buffer;

/* 数码管对象 */
smg_t smg;

static int xuehao = 0;

static void init_smg(void);
static void uart_task(void*);
static void key_task(void*);

// 最小光照强度时的ADC值
#define MIN_LIGHT_LEVEL  3000
// 最大光照强度时的ADC值
#define MAX_LIGHT_LEVEL  120

/**
 * @brief 应用程序入口点
 */
void app_main()
{
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 0);
  HAL_ADCEx_Calibration_Start(&hadc1);
  init_smg();
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_buffer, 1);

  xTaskCreate(uart_task, "uart", 128, NULL, 0, NULL);
  xTaskCreate(key_task, "keys", 128, NULL, 0, NULL);

  HAL_TIM_Base_Start_IT(&htim1);

  char buf[32] = {0};
  while(1) {
    vTaskDelay(pdMS_TO_TICKS(250));
    snprintf(buf, sizeof(buf), "\u5149\u654f\u7535\u963b: %.2f\r\n", 3.3F * adc_buffer / 4096);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, sizeof(buf), 100);
  }
}


/**
 * @brief 按键扫描任务函数
 * @param args 
 */
static void key_task(void* args) {
  while (1) {
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == 0) {
      vTaskDelay(pdMS_TO_TICKS(10));
      if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == 0) {
        xuehao = 0;
      }
      while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == 0);
    } else if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == 0) {
      vTaskDelay(pdMS_TO_TICKS(10));
      if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == 0) {
        xuehao = 1;
      }
      while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == 0);
    }
  }
}

static void init_smg() {
  hc138_t hc138 = {
    .a0 = {GPIOC, GPIO_PIN_10},		// 低位
    .a1 = {GPIOC, GPIO_PIN_11},		
    .a2 = {GPIOC, GPIO_PIN_12},		// 高位
  };

  hc595_t hc595 = {
    .sclk = {GPIOB, GPIO_PIN_5},	// 上升沿下一个数据
    .lclk = {GPIOB, GPIO_PIN_4},	// 上升沿并行输
    .ds = {GPIOB, GPIO_PIN_3}		// 数据输入
  };
  smg_init(&hc138, &hc595, &smg);
}

static void uart_task(void* args) {
  char smg_buf[9] = {0};
  while (1) {
    smg_from_string(&smg, "________");
    snprintf(smg_buf, sizeof(smg_buf), "%8.2f____", 3.3f * adc_buffer / 4095);
    smg_from_string(&smg, smg_buf);

    float step = (float)(MIN_LIGHT_LEVEL - MAX_LIGHT_LEVEL) / 8;

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, adc_buffer > MAX_LIGHT_LEVEL + step * 0);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, adc_buffer > MAX_LIGHT_LEVEL + step * 1);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, adc_buffer > MAX_LIGHT_LEVEL + step * 2);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, adc_buffer > MAX_LIGHT_LEVEL + step * 3);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, adc_buffer > MAX_LIGHT_LEVEL + step * 4);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, adc_buffer > MAX_LIGHT_LEVEL + step * 5);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, adc_buffer > MAX_LIGHT_LEVEL + step * 6);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, adc_buffer > MAX_LIGHT_LEVEL + step * 7);

    vTaskDelay(pdMS_TO_TICKS(100)); 
  }
}