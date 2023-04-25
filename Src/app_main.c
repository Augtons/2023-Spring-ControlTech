#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "smg.h"

#define loop for(;;)

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim1;

/* 数码管对象 */
smg_t smg;

static void init_smg(void);
static void uart_task(void*);
static void key_task(void*);

/**
 * @brief 应用程序入口点
 */
void app_main()
{
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 0);
  init_smg();

  xTaskCreate(uart_task, "uart", 128, NULL, 0, NULL);
  xTaskCreate(key_task, "keys", 128, NULL, 0, NULL);

  HAL_TIM_Base_Start_IT(&htim1);

  char buf[32] = {0};
  while(1) {
    vTaskDelay(pdMS_TO_TICKS(250));
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

      }
      while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == 0);
    } else if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == 0) {
      vTaskDelay(pdMS_TO_TICKS(10));
      if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == 0) {

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

    vTaskDelay(pdMS_TO_TICKS(100)); 
  }
}
