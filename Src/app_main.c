#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

#define loop for(;;)

enum color_mode_t {
  COLOR_MODE_NONE,
  COLOR_MODE_WHITE_MODE,
  COLOR_MODE_RAINBOW_MODE
};

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim2;

static void key_task(void*);
static void led_mode_white(void*);
static TaskHandle_t led_white_mode_task_handle;
static void led_mode_rainbow(void*);
static TaskHandle_t led_rainbow_mode_task_handle;

// 刷新LED灯颜色
static void flush_leds(uint32_t d1, uint32_t d2, uint32_t d3);
// 改变LED灯模式
static void led_change_mode(enum color_mode_t mode);
// HSV转RGB
void hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b);

/**
 * @brief 应用程序入口点
 */
void app_main()
{
  xTaskCreate(key_task, "keys", 128, NULL, 0, NULL);
  xTaskCreate(led_mode_white, "led_mode_white", 128, NULL, 0, &led_white_mode_task_handle);
  xTaskCreate(led_mode_rainbow, "led_mode_rainbow", 128, NULL, 0, &led_rainbow_mode_task_handle);
  //xTaskCreate(led_mode3, "led_mode3", 128, NULL, 0, NULL);

  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);

  __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 500);
  __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 500);
  __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 500);

  led_change_mode(COLOR_MODE_RAINBOW_MODE);
  //vTaskSuspend(NULL);

  uint8_t pins = 0x01;
  while(1) {
    if (pins == 0) {
      pins = 0x01;
    }
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, !((pins >> 0) & 1));
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, !((pins >> 1) & 1));
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, !((pins >> 2) & 1));
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, !((pins >> 3) & 1));
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, !((pins >> 4) & 1));
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, !((pins >> 5) & 1));
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, !((pins >> 6) & 1));
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, !((pins >> 7) & 1));

    
    pins <<= 1;
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

static void led_change_mode(enum color_mode_t mode) {
  vTaskSuspend(led_white_mode_task_handle);
  vTaskSuspend(led_rainbow_mode_task_handle);
  
  if (mode == COLOR_MODE_WHITE_MODE) {
    vTaskResume(led_white_mode_task_handle);
  } else if (mode == COLOR_MODE_RAINBOW_MODE) {
    vTaskResume(led_rainbow_mode_task_handle);
  } else {

  }
}

static void led_mode_white(void* args) {
  char direction = 1;
  uint32_t led_duty1 = 0;
  uint32_t led_duty2 = 0;
  uint32_t led_duty3 = 0;
  while (1) {
    vTaskDelay(pdMS_TO_TICKS(5));
    
    if (direction > 0) {
      led_duty1 += 10; led_duty2 += 10; led_duty3 += 10;
    }else{
      led_duty1 -= 10; led_duty2 -= 10; led_duty3 -= 10;
    }
    
    if(direction > 0 && led_duty1 >= 1000) {
      direction = 0;
    } else if (direction <= 0 && led_duty1 <= 0) {
      direction = 1;
    }

    flush_leds(led_duty1, led_duty2, led_duty3);
  }
  
}

static void led_mode_rainbow(void* args) {
  uint16_t phrase = 0;
  uint32_t led_duty1 = 0;
  uint32_t led_duty2 = 0;
  uint32_t led_duty3 = 0;

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(10));
    if (++phrase > 360) { phrase = 0; }
    hsv2rgb(phrase, 100, 100, &led_duty1, &led_duty2, &led_duty3);
    led_duty1 = 1000 - (led_duty1 * 1000 / 255);
    led_duty2 = 1000 - (led_duty2 * 1000 / 255);
    led_duty3 = 1000 - (led_duty3 * 1000 / 255);
    flush_leds(led_duty1, led_duty2, led_duty3);
  }
}
// static void led_mode3(void* args) {
//   while (1) {

//   }
// }

#define KEY_EVENT(port, pin, callback)       \
  if (HAL_GPIO_ReadPin(port, pin) == 0) {    \
    vTaskDelay(pdMS_TO_TICKS(10));           \
    if (HAL_GPIO_ReadPin(port, pin) == 0) {  \
      callback;                              \
    }                                        \
    while(HAL_GPIO_ReadPin(port, pin) == 0); \
  }

/**
 * @brief 按键扫描任务函数
 * @param args 
 */
static void key_task(void* args) {
  char mode = 0;
  while (1) {
    vTaskDelay(pdMS_TO_TICKS(10));
    // 按键 0
    KEY_EVENT(KEY0_GPIO_Port, KEY0_Pin, {
      led_change_mode(COLOR_MODE_WHITE_MODE);
    });
    // 按键 1
    KEY_EVENT(KEY1_GPIO_Port, KEY1_Pin, {
      led_change_mode(COLOR_MODE_RAINBOW_MODE);
      mode = !mode;
      __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, (mode ? 800 : 0));
    });
    // 按键3 
    KEY_EVENT(KEY3_GPIO_Port, KEY3_Pin, {
      //mode = !mode;
      //__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, (mode ? 800 : 0));
    });
  }
}

#undef KEY_EVENT

static void flush_leds(uint32_t d1, uint32_t d2, uint32_t d3) {
  __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, d1);
  __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, d2);
  __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, d3);
}

void hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b)
{
  h %= 360; // h -> [0,360]
  uint32_t rgb_max = v * 2.55f;
  uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

  uint32_t i = h / 60;
  uint32_t diff = h % 60;

  uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

  switch (i) {
    case 0:
      *r = rgb_max;
      *g = rgb_min + rgb_adj;
      *b = rgb_min;
      break;
    case 1:
      *r = rgb_max - rgb_adj;
      *g = rgb_max;
      *b = rgb_min;
      break;
    case 2:
      *r = rgb_min;
      *g = rgb_max;
      *b = rgb_min + rgb_adj;
      break;
    case 3:
      *r = rgb_min;
      *g = rgb_max - rgb_adj;
      *b = rgb_max;
      break;
    case 4:
      *r = rgb_min + rgb_adj;
      *g = rgb_min;
      *b = rgb_max;
      break;
    default:
      *r = rgb_max;
      *g = rgb_min;
      *b = rgb_max - rgb_adj;
      break;
  }
}