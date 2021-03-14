#include "bsp.h"
#include "falcon_common.h"
#include "stm32f4xx_it.h"

#include "logger.h"
#include "device_com.h"
#include "falcon_packet.h"
#include "ff_encoder.h"

#include <stdint.h>
#include <string.h>

#define logger_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define device_com_TASK_PRIORITY  (tskIDLE_PRIORITY + 2)

static uint8_t startedOS= 0;

static void startOS(void)
{
  startedOS = 1;
  vTaskStartScheduler();
}

void albus_delay(uint32_t ms)
{
  if (startedOS) {
    vTaskDelay(MS_TO_TICKS(ms));
  }
  else {
    HAL_Delay(ms);
  }
}

void albus_sysTickHandler(void)
{
  if (startedOS) {
    OSSysTickHandler();
  }
}

void sendDataTask(void *arg)
{
  uint8_t frame_buffer[MAX_FRAME_SIZE] = {0};
  ff_encoder_t encoder;
  ff_encoder_init(&encoder);
  ff_encoder_set_buffer(&encoder, frame_buffer);

  fpc_motor_speed_t motorSpeedCmd = {
    .pwmData = {
      0,
      250,
      500,
      750
    }
  };

  fpc_flight_control_position_t positionReferenceCmd = {
    .positionReferenceCMD = {
      1.2,
      2.4,
      3.6,
      4.8
    }
  };

  if (ff_encoder_append_packet(&encoder, &motorSpeedCmd, FPT_MOTOR_SPEED_COMMAND) == FLN_ERR) {
    error_handler();
  }

  if (ff_encoder_append_packet(&encoder, &positionReferenceCmd, FPT_FLIGHT_CONTROL_POSITION_COMMAND) == FLN_ERR) {
    error_handler();
  }

  ff_encoder_append_footer(&encoder);

  while(1) {
    device_com_send_frame(frame_buffer);
    vTaskDelay(3000);
  }
}

int main(void)
{
  int32_t taskStatus;
  bsp_board_bringup();
  logger_init();

  DEBUG_LOG("Albus Started\r\n");

  device_com_setup();

  BSP_LED_Init(LED1);
  BSP_LED_On(LED1);
  taskStatus = xTaskCreate(device_com_task,
                        "device_com_task",
                        4*configMINIMAL_STACK_SIZE,
                        NULL,
                        device_com_TASK_PRIORITY,
                        NULL);

  RTOS_ERR_CHECK(taskStatus);

  taskStatus = xTaskCreate(sendDataTask,
                        "senddatt",
                        4*configMINIMAL_STACK_SIZE,
                        NULL,
                        device_com_TASK_PRIORITY+3,
                        NULL);

  RTOS_ERR_CHECK(taskStatus);

  DEBUG_LOG("Starting tasks... \r\n");

  startOS();
  /* Should never reach here */
  while (1);
}
