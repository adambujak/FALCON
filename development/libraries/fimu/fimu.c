/******************************************************************************
 * @file     fimu.c 
 * @brief    Falcon IMU
 * @version  1.0
 * @date     2020-11-02
 * @author   Devin Bell
 ******************************************************************************/

#include "fimu.h"

signed char ACCEL_GYRO_ORIENTATION[9] = {0,1,0,
                                         -1,0,0,
                                         0,0,1};
signed char COMPASS_ORIENTATION[9] = {1,0,0,
                                      0,-1,0,
                                      0,0,-1};
const unsigned char ACCEL_GYRO_CHIP_ADDR = 0x68;
const unsigned char COMPASS_SLAVE_ID = HW_AK09916;
const unsigned char COMPASS_CHIP_ADDR = 0x0C;
const unsigned char PRESSURE_CHIP_ADDR = 0x00;
long SOFT_IRON_MATRIX[] = {0,0,0,0,0,0,0,0,0};
int a_average[3];
int g_average[3];

int fimu_init(fimu_config_t config)
{
  int result = 0;
  ICM_connect_i2c(config.i2cHandle);
  inv_sleep(10);
  inv_set_chip_to_body_axis_quaternion(ACCEL_GYRO_ORIENTATION, 0.0);
  result |= inv_initialize_lower_driver(SERIAL_INTERFACE_I2C, 0);
  result |= inv_set_slave_compass_id(COMPASS_SLAVE_ID);
  if (result) {
      DEBUG_LOG("Could not initialize.\r\n");
      return FLN_ERR;
  }
  else {
      DEBUG_LOG("Initialized.\r\n");
      return FLN_OK;
  }

  return FLN_OK;
}

int fimu_start(fimu_config_t config)
{
  int result = 0;

  result |= inv_set_gyro_divider(1U);       //Initial sampling rate 1125Hz/10+1 = 102Hz.
  result |= inv_set_accel_divider(1U);      //Initial sampling rate 1125Hz/10+1 = 102Hz.
  result |= inv_set_gyro_fullscale(config.gyro_fsr);
  result |= inv_set_accel_fullscale(config.accel_fsr);
  result |= inv_enable_sensor(ANDROID_SENSOR_GYROSCOPE, 1);
  result |= inv_enable_sensor(ANDROID_SENSOR_LINEAR_ACCELERATION, 1);
  result |= inv_enable_sensor(ANDROID_SENSOR_ROTATION_VECTOR, 1);
  unsigned short data_output_delay_ms = (unsigned short)( 1125 / config.output_data_rate );
  result |= inv_set_odr(ANDROID_SENSOR_GYROSCOPE, data_output_delay_ms);
  result |= inv_set_odr(ANDROID_SENSOR_LINEAR_ACCELERATION, data_output_delay_ms);
  result |= inv_set_odr(ANDROID_SENSOR_ROTATION_VECTOR, data_output_delay_ms);

  result |= inv_reset_dmp_odr_counters();
  result |= dmp_reset_fifo();

  return result;
}

void fimu_fifo_handler(float *gyro_float, float *linAccFloat, float *rv_float)
{
  short int_read_back = 0;
  unsigned short header = 0, header2 = 0;
  int data_left_in_fifo = 0;
  short short_data[3] = { 0 };
  signed long  long_data[3] = { 0 };
  signed long  long_quat[3] = { 0 };
  unsigned short sample_cnt_array[GENERAL_SENSORS_MAX] = { 0 };
  
  float accel_float[3];

  // Process Incoming INT and Get/Pack FIFO Data
  inv_identify_interrupt(&int_read_back);
  if (int_read_back & (BIT_MSG_DMP_INT | BIT_MSG_DMP_INT_0 | BIT_MSG_DMP_INT_2 | BIT_MSG_DMP_INT_5)) {

    // Read FIFO contents and parse it.
    unsigned short total_sample_cnt = 0;

    do {
      if (inv_mems_fifo_swmirror(&data_left_in_fifo, &total_sample_cnt, sample_cnt_array))
        break;

      while (total_sample_cnt--){
        if (inv_mems_fifo_pop(&header, &header2, &data_left_in_fifo))
          break;

        if (header & ACCEL_SET) {
          float scale;
          dmp_get_accel(long_data);
          scale = (1 << inv_get_accel_fullscale()) * 2.f / (1L << 30); // Convert from raw units to g's
          scale *= 9.80665f; // Convert to m/s^2
          inv_convert_dmp3_to_body(long_data, scale, accel_float);
        } // header & ACCEL_SET

        if (header & GYRO_SET) {
          float scale;
          signed long  raw_data[3] = { 0 };
          signed long  bias_data[3] = { 0 };
          float gyro_bias_float[3] = { 0 };
          float gyro_raw_float[3] = { 0 };

          dmp_get_raw_gyro(short_data);
          scale = (1 << inv_get_gyro_fullscale()) * 250.f / (1L << 15); // From raw to dps
          scale *= (float)M_PI / 180.f; // Convert to radian.
          raw_data[0] = (long)short_data[0];
          raw_data[1] = (long)short_data[1];
          raw_data[2] = (long)short_data[2];
          inv_convert_dmp3_to_body(raw_data, scale, gyro_raw_float);

          // We have gyro bias data in raw units, scaled by 2^5
          dmp_get_gyro_bias(short_data);
          scale = (1 << inv_get_gyro_fullscale()) * 250.f / (1L << 20); // From raw to dps
          scale *= (float)M_PI / 180.f; // Convert to radian.
          bias_data[0] = (long)short_data[0];
          bias_data[1] = (long)short_data[1];
          bias_data[2] = (long)short_data[2];
          inv_convert_dmp3_to_body(bias_data, scale, gyro_bias_float);

          // shift to Q20 to do all operations in Q20
          raw_data[0] = raw_data[0] << 5;
          raw_data[1] = raw_data[1] << 5;
          raw_data[2] = raw_data[2] << 5;
          inv_mems_dmp_get_calibrated_gyro(long_data, raw_data, bias_data);
          inv_convert_dmp3_to_body(long_data, scale, gyro_float);          
        } // header & GYRO_SET

        if (header & QUAT6_SET) {
          dmp_get_6quaternion(long_quat);

          long gravityQ16[3], temp_gravityQ16[3];
          long linAccQ16[3];
          long accelQ16[3];

          /*Calculate Gravity*/
          inv_convert_rotation_vector_1(long_quat, temp_gravityQ16);
          inv_mems_augmented_sensors_get_gravity(gravityQ16, temp_gravityQ16);

          /*Calculate Linear Acceleration*/
          accelQ16[0] = (int32_t)((float)(accel_float[0])*(1ULL << 16) + ((accel_float[0] >= 0) - 0.5f));
          accelQ16[1] = (int32_t)((float)(accel_float[1])*(1ULL << 16) + ((accel_float[1] >= 0) - 0.5f));
          accelQ16[2] = (int32_t)((float)(accel_float[2])*(1ULL << 16) + ((accel_float[2] >= 0) - 0.5f));
          inv_mems_augmented_sensors_get_linearacceleration(linAccQ16, gravityQ16, accelQ16);
          linAccFloat[0] = inv_q16_to_float(linAccQ16[0]);
          linAccFloat[1] = inv_q16_to_float(linAccQ16[1]);
          linAccFloat[2] = inv_q16_to_float(linAccQ16[2]);
        } // header & QUAT6_SET

        if (header & QUAT9_SET) {
          dmp_get_9quaternion(long_quat);

          inv_convert_rotation_vector(long_quat, rv_float);
        } // header & QUAT9_SET

      } // total_sample_cnt

      if (!data_left_in_fifo)
        break;
    } while (data_left_in_fifo);
  }
  else return;
}

int fimu_calibrate(void)
{
  DEBUG_LOG("Selftest started.\r\n");

  int self_test_result = 0;
  int dmp_bias[9] = { 0 };
  self_test_result = inv_mems_run_selftest();

  DEBUG_LOG("Selftest...Done...Ret=%d\r\n", self_test_result);
  DEBUG_LOG("Result: Compass=%s, Accel=%s, Gyro=%s\r\n", (self_test_result & 0x04) ? "Pass" : "Fail", (self_test_result & 0x02) ? "Pass" : "Fail", (self_test_result & 0x01) ? "Pass" : "Fail");
  DEBUG_LOG("Accel Average (LSB@FSR 2g)\r\n");
  DEBUG_LOG("\tX:%d Y:%d Z:%d\r\n", a_average[0], a_average[1], a_average[2]);
  DEBUG_LOG("Gyro Average (LSB@FSR 250dps)\r\n");
  DEBUG_LOG("\tX:%d Y:%d Z:%d\r\n", g_average[0], g_average[1], g_average[2]);

  if ((self_test_result & 0x03) != 0x03) {
    return FLN_ERR;
  }

  dmp_bias[0] = a_average[0] * (1 << 11);   // Change from LSB to format expected by DMP
  dmp_bias[1] = a_average[1] * (1 << 11);
  dmp_bias[2] = (a_average[2] - 16384) * (1 << 11); //remove the gravity and scale (FSR=2 in selftest)
  int scale = 2000 / 250; //self-test uses 250dps FSR, main() set the FSR to 2000dps
  dmp_bias[3] = g_average[0] * (1 << 15) / scale;
  dmp_bias[4] = g_average[1] * (1 << 15) / scale;
  dmp_bias[5] = g_average[2] * (1 << 15) / scale;

  dmp_set_bias(dmp_bias);

  return FLN_OK;
}
