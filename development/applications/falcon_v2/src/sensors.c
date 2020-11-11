#include "sensors.h"

#include "falcon_common.h"
#include "bsp.h"
#include "logger.h"
#include "fimu.h"

fln_i2c_handle_t i2cHandle;

#define MAX_BUF_LENGTH  (18)

enum packet_type_e {
  PACKET_TYPE_ACCEL,
  PACKET_TYPE_GYRO,
  PACKET_TYPE_QUAT,
  PACKET_TYPE_COMPASS = 7
};

volatile unsigned char new_gyro;
void gyro_data_ready_cb(void)
{
  new_gyro = 1;
}

void sensors_task_setup(void)
{
  FLN_ERR_CHECK(bsp_i2c_init(&i2cHandle));
}

void sensors_task(void *pvParameters)
{
	DEBUG_LOG("TASK STARTED\r\n");

	/* Send data to the Python client application.
  * Data is formatted as follows:
  * packet[0]    = $
  * packet[1]    = packet type (see packet_type_e)
  * packet[2+]   = data
  */
  void send_data_packet(char packet_type, void *data)
  {
    long *ldata = (long*)data;
    char buf[MAX_BUF_LENGTH], length = 0;

    memset(buf, 0, MAX_BUF_LENGTH);
    buf[0] = '$';
    buf[1] = packet_type;

    switch (packet_type)
    {
    case PACKET_TYPE_ACCEL:
    case PACKET_TYPE_GYRO:
    case PACKET_TYPE_COMPASS:
      buf[2] = (char)(ldata[0] >> 24);
      buf[3] = (char)(ldata[0] >> 16);
      buf[4] = (char)(ldata[0] >> 8);
      buf[5] = (char)ldata[0];
      buf[6] = (char)(ldata[1] >> 24);
      buf[7] = (char)(ldata[1] >> 16);
      buf[8] = (char)(ldata[1] >> 8);
      buf[9] = (char)ldata[1];
      buf[10] = (char)(ldata[2] >> 24);
      buf[11] = (char)(ldata[2] >> 16);
      buf[12] = (char)(ldata[2] >> 8);
      buf[13] = (char)ldata[2];
      length = 14;
      break;
    case PACKET_TYPE_QUAT:
      buf[2] = (char)(ldata[0] >> 24);
      buf[3] = (char)(ldata[0] >> 16);
      buf[4] = (char)(ldata[0] >> 8);
      buf[5] = (char)ldata[0];
      buf[6] = (char)(ldata[1] >> 24);
      buf[7] = (char)(ldata[1] >> 16);
      buf[8] = (char)(ldata[1] >> 8);
      buf[9] = (char)ldata[1];
      buf[10] = (char)(ldata[2] >> 24);
      buf[11] = (char)(ldata[2] >> 16);
      buf[12] = (char)(ldata[2] >> 8);
      buf[13] = (char)ldata[2];
      buf[14] = (char)(ldata[3] >> 24);
      buf[15] = (char)(ldata[3] >> 16);
      buf[16] = (char)(ldata[3] >> 8);
      buf[17] = (char)ldata[3];
      length = 18;
      break;
    }
    if (length) {
      for (int ii = 0; ii < length; ii++)
        logger_put_char(buf[ii]);
    }
  }

  long quat_out[4];

  fimu_config_t IMU_config = {
    .i2cHandle = &i2cHandle,
    .gyro_fsr = MPU_FS_2000dps,
    .accel_fsr = MPU_FS_2G,
    .output_data_rate = 200
  };

  FLN_ERR_CHECK(fimu_init(IMU_config));

  FLN_ERR_CHECK(fimu_calibrate());

  bsp_IMU_int_init(gyro_data_ready_cb);

  FLN_ERR_CHECK(fimu_start(IMU_config));

  while (1) {

    if (new_gyro == 1) {
      new_gyro = 0;
      fimu_fifo_handler(sensor_data.gyro_data_SI, sensor_data.accel_data_SI, sensor_data.quat_data);
//      DEBUG_LOG("Gyro Data\t %7.5f, %7.5f, %7.5f\r\n", sensor_data.gyro_data_SI[0], sensor_data.gyro_data_SI[1], sensor_data.gyro_data_SI[2]);
//      DEBUG_LOG("Accel Data\t %7.5f, %7.5f, %7.5f\r\n", sensor_data.accel_data_SI[0], sensor_data.accel_data_SI[1], sensor_data.accel_data_SI[2]);
//      quat_out[0] = (long)(sensor_data.quat_data[0] * 65536.f);
//      quat_out[1] = (long)(sensor_data.quat_data[1] * 65536.f);
//      quat_out[2] = (long)(sensor_data.quat_data[2] * 65536.f);
//      quat_out[3] = (long)(sensor_data.quat_data[3] * 65536.f);
//      send_data_packet(PACKET_TYPE_QUAT, (void *)quat_out);
//      DEBUG_LOG("%7.5f\t%7.5f\t%7.5f\t%7.5f\r\n", sensor_data.quat_data[0], -sensor_data.quat_data[3], sensor_data.quat_data[2], -sensor_data.quat_data[1]);
      DEBUG_LOG("%7.5f\t%7.5f\t%7.5f\t%7.5f\r\n", sensor_data.quat_data[0], sensor_data.quat_data[1], sensor_data.quat_data[2], sensor_data.quat_data[3]);
      HAL_GPIO_TogglePin(GPIOE, 4U);
      HAL_GPIO_TogglePin(GPIOE, 4U);
    }
  }
}
