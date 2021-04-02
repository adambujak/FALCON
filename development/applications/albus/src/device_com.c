#include "device_com.h"

#include "falcon_common.h"

#include "radio.h"
#include "uart.h"

#include "falcon_packet.h"
#include "fs_decoder.h"
#include "fp_decode.h"

static uint8_t uart_rx_buffer[MAX_FRAME_SIZE];
static fs_decoder_t decoder;

static void decoder_callback(uint8_t *data, fp_type_t packetType)
{
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wswitch"
  switch (packetType) {
  case FPT_FLIGHT_CONTROL_COMMAND:
  {
    fpc_flight_control_t control = {{0}};
    fpc_flight_control_decode(data, &control);
    LOG_DEBUG("received motor cmd\r\n");
    LOG_DEBUG("MOTOR COMMAND: %f, %f, %f, %f\r\n",
           control.fcsControlCmd.yaw,
           control.fcsControlCmd.pitch,
           control.fcsControlCmd.roll,
           control.fcsControlCmd.alt);
  }
    break;
//    case FPT_SET_DESTINATION_COMMAND:
//    {
//      fpc_set_destination_t dest = {0};
//      fpc_set_destination_decode(data, &dest);
//      printf("decoded dest: %f\r\n", dest.gpsData.latitude);
//    }
//      break;
//    case FPT_FLIGHT_CONTROL_POSITION_COMMAND:
//    {
//      fpc_flight_control_position_t pos = {0};
//      fpc_flight_control_position_decode(data, &pos);
//      printf("decoded posi: %f %f %f %f\r\n", pos.positionReferenceCMD.x, pos.positionReferenceCMD.y, pos.positionReferenceCMD.z, pos.positionReferenceCMD.yaw);
//    }
//      break;
//    default:
//      break;
  }
  #pragma GCC diagnostic pop
}

static void decode_frame(uint8_t *data, uint32_t length)
{
  fs_decoder_decode(&decoder, data, length);
}

static void decoder_init(void)
{
   fs_decoder_config_t decoder_config = {
     .callback = decoder_callback
   };
   fs_decoder_init(&decoder, &decoder_config);
}

static void handle_uart(void)
{
  if (uart_read(uart_rx_buffer, MAX_FRAME_SIZE) == MAX_FRAME_SIZE) {
    decode_frame(uart_rx_buffer, MAX_FRAME_SIZE);
    LOG_DEBUG("received frame\r\n");
  }
}

void device_com_task(void *pvParameters)
{
  LOG_DEBUG("Device com task started\r\n");

  while(1) {
    handle_uart();
    vTaskDelay(100);
  }
}

void device_com_setup(void)
{
  decoder_init();
  radio_init();
}

void device_com_start(void)
{
  int32_t taskStatus;

  taskStatus = xTaskCreate(device_com_task,
                        "device_com_task",
                        configMINIMAL_STACK_SIZE*4,
                        NULL,
                        tskIDLE_PRIORITY + 1,
                        NULL);

  ASSERT(taskStatus == pdTRUE);
}
