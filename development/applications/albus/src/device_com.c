#include "device_com.h"

#include "falcon_common.h"

#include "radio.h"
#include "uart.h"

#include "falcon_packet.h"
// #include "fs_decoder.h"
// #include "fp_decode.h"


// #define RTOS_TIMEOUT_TICKS 25

// typedef enum {
//   UART_STATE_IDLE = 0,
//   UART_STATE_READING,
//   UART_STATE_READY,
// } uart_state_t;



// static uint8_t uartBuffer[MAX_FRAME_SIZE];

// static fs_decoder_t decoder;



// static volatile bool rfTxReady = false;
// static volatile bool rfRxReady = false;
// static volatile uart_state_t uartState = UART_STATE_IDLE;
// static TimerHandle_t rfTxTimer;
// static SemaphoreHandle_t rfTxReadyMutex;
// static SemaphoreHandle_t frfMutex;
// static SemaphoreHandle_t uartMutex;


// static void decoder_callback(uint8_t *data, fp_type_t packetType)
// {
//   #pragma GCC diagnostic push
//   #pragma GCC diagnostic ignored "-Wswitch"
//   switch (packetType) {
//   case FPT_FLIGHT_CONTROL_COMMAND:
//   {
//     fpc_flight_control_t control = {{0}};
//     fpc_flight_control_decode(data, &control);
//     DEBUG_LOG("received motor cmd\r\n");
//     DEBUG_LOG("MOTOR COMMAND: %f, %f, %f, %f\r\n",
//            control.fcsControlCmd.yaw,
//            control.fcsControlCmd.pitch,
//            control.fcsControlCmd.roll,
//            control.fcsControlCmd.alt);
//   }
//     break;
// //    case FPT_SET_DESTINATION_COMMAND:
// //    {
// //      fpc_set_destination_t dest = {0};
// //      fpc_set_destination_decode(data, &dest);
// //      printf("decoded dest: %f\r\n", dest.gpsData.latitude);
// //    }
// //      break;
// //    case FPT_FLIGHT_CONTROL_POSITION_COMMAND:
// //    {
// //      fpc_flight_control_position_t pos = {0};
// //      fpc_flight_control_position_decode(data, &pos);
// //      printf("decoded posi: %f %f %f %f\r\n", pos.positionReferenceCMD.x, pos.positionReferenceCMD.y, pos.positionReferenceCMD.z, pos.positionReferenceCMD.yaw);
// //    }
// //      break;
// //    default:
// //      break;
//   }
//   #pragma GCC diagnostic pop
// }

// static void decode_frame(uint8_t *data, uint32_t length)
// {
//   fs_decoder_decode(&decoder, data, length);
// }

// static void decoder_init(void)
// {
//    fs_decoder_config_t decoder_config = {
//      .callback = decoder_callback
//    };
//    fs_decoder_init(&decoder, &decoder_config);
// }

// static inline void createUartMutex(void)
// {
//   uartMutex = xSemaphoreCreateMutex();
//   if (uartMutex == NULL) {
//     error_handler();
//   }
// }

// static inline uint32_t lock_uart(void)
// {
//   return xSemaphoreTake(uartMutex, RTOS_TIMEOUT_TICKS);
// }

// static inline void unlock_uart(void)
// {
//   xSemaphoreGive(uartMutex);
// }

// static void uart_rx_callback(void)
// {
//     uartState = UART_STATE_READY;
//     DEBUG_LOG("data\r\n");
// }





// static inline uint32_t lock_rf_tx_ready_flag(void)
// {
//   return xSemaphoreTake(rfTxReadyMutex, RTOS_TIMEOUT_TICKS);
// }

// static inline void unlock_rf_tx_ready_flag(void)
// {
//   xSemaphoreGive(rfTxReadyMutex);
// }

// static inline void createRFTxReadyMutex(void)
// {
//   rfTxReadyMutex = xSemaphoreCreateMutex();
//   if (rfTxReadyMutex == NULL) {
//     error_handler();
//   }
// }

// static inline uint32_t lock_frf(void)
// {
//   return xSemaphoreTake(frfMutex, RTOS_TIMEOUT_TICKS);
// }

// static inline void unlock_frf(void)
// {
//   xSemaphoreGive(frfMutex);
// }

// static inline void createFRFMutex(void)
// {
//   frfMutex = xSemaphoreCreateMutex();
//   if (frfMutex == NULL) {
//     error_handler();
//   }
// }

// static inline void setRFTxReadyFlag(bool val)
// {
//   if (lock_rf_tx_ready_flag() == pdTRUE) {
//     rfTxReady = val;
//     unlock_rf_tx_ready_flag();
//   }
// }

// static inline bool getRFTxReadyFlag(void)
// {
//   bool retval = false;
//   if (lock_rf_tx_ready_flag() == pdTRUE) {
//     retval = rfTxReady;
//     unlock_rf_tx_ready_flag();
//   }
//   return retval;
// }

// static void rfTxTimerCallback(TimerHandle_t timer)
// {
//   setRFTxReadyFlag(true);
// }

// static inline void createRFTxTimer(void)
// {
//   rfTxTimer = xTimerCreate("rfTxTimer",
//                     TICKS_TO_MS(FRF_DEFAULT_TX_TIMER_PERIOD_MS*80),
//                     pdTRUE,
//                     (void *) 0,
//                     rfTxTimerCallback
//                     );
//   xTimerStart(rfTxTimer, RTOS_TIMEOUT_TICKS);
// }

// static inline void handleRFTx(void)
// {
//   if (getRFTxReadyFlag()) {
//     frf_tx(&radio);
//     setRFTxReadyFlag(false);
//   }
// }

// static inline void handleRFRx(void)
// {
//   if (rfRxReady) {
//     frf_packet_t packet;
//     if (frf_getPacket(&radio, packet) == 0) {
//       DEBUG_LOG("HEDWIG: %s\r\n", (char*)packet);
//     }
//     rfRxReady = false;
//   }
// }

// static inline void rfProcess(void)
// {
//   if (lock_frf() == pdTRUE) {
//     //handleRFRx();
//     handleRFTx();
//     frf_process(&radio);
//     unlock_frf();
//   }
// }



// void device_com_send_frame(uint8_t *frame)
// {
//   if (lock_frf() == pdTRUE) {
//     for (uint8_t i = 0; i < MAX_FRAME_SIZE; i += FRF_PACKET_SIZE) {
//       frf_pushPacket(&radio, &frame[i]);
//       DEBUG_LOG("frame send\r\n");
//     }
//     unlock_frf();
//   }
// }

static uint8_t uart_rx_buffer[MAX_FRAME_SIZE];

static void handle_uart(void)
{
  if (uart_read(uart_rx_buffer, MAX_FRAME_SIZE) == MAX_FRAME_SIZE) {
    LOG_DEBUG("recievied frame\r\n");
  }
}

void device_com_task(void *pvParameters)
{
  LOG_DEBUG("Device com task started\r\n");
  radio_init();

  while(1) {
    // radio_process();
    handle_uart();
    vTaskDelay(250);
  }
}

void device_com_setup(void)
{

  // decoder_init();

  // createFRFMutex();
  // createRFTxTimer();
  // createRFTxReadyMutex();
  // createUartMutex();
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
