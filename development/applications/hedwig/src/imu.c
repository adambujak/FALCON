#include "imu.h"

#include "falcon_common.h"
#include "inv_mems.h"

signed char ACCEL_GYRO_ORIENTATION[9] = {0, 1, 0, 1, 0, 0, 0, 0, -1};
signed char COMPASS_ORIENTATION[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

const unsigned char ACCEL_GYRO_CHIP_ADDR = 0x68;
const unsigned char COMPASS_SLAVE_ID = HW_AK09916;
const unsigned char COMPASS_CHIP_ADDR = 0x0C;
const unsigned char PRESSURE_CHIP_ADDR = 0x00;
long SOFT_IRON_MATRIX[] = {1073741824,0,0,0,1073741824,0,0,0,1073741824};

int imu_calibrate(float *gyro_bias, float *accel_bias)
{
  // TODO: add this
  return FLN_OK;
}

int imu_get_data(float *accel_float, float *gyro_float, float *compass_float)
{
  short int_read_back = 0;
  unsigned short header = 0, header2 = 0;
  int data_left_in_fifo = 0;
  short short_data[3] = {0};
  signed long long_data[3] = {0};
  unsigned short sample_cnt_array[GENERAL_SENSORS_MAX] = {0};

  // Process Incoming INT and Get/Pack FIFO Data
  inv_identify_interrupt(&int_read_back);
  if (int_read_back & (BIT_MSG_DMP_INT | BIT_MSG_DMP_INT_0 | BIT_MSG_DMP_INT_2 | BIT_MSG_DMP_INT_5)) {
    // Read FIFO contents and parse it.
    unsigned short total_sample_cnt = 0;

    do {
      if (inv_mems_fifo_swmirror(&data_left_in_fifo, &total_sample_cnt, sample_cnt_array)) break;

      while (total_sample_cnt--) {
        if (inv_mems_fifo_pop(&header, &header2, &data_left_in_fifo)) break;

        if (header & ACCEL_SET) {
          float scale;
          dmp_get_accel(long_data);
          scale = (1 << inv_get_accel_fullscale()) * 2.f / (1L << 30);  // Convert from raw units to g's
          scale *= 9.80665f;                                            // Convert to m/s^2
          inv_convert_dmp3_to_body(long_data, scale, accel_float);
        }  // header & ACCEL_SET

        if (header & GYRO_SET) {
          float scale;
          signed long raw_data[3] = {0};
          signed long bias_data[3] = {0};
          float gyro_bias_float[3] = {0};
          float gyro_raw_float[3] = {0};

          dmp_get_raw_gyro(short_data);
          scale = (1 << inv_get_gyro_fullscale()) * 250.f / (1L << 15);  // From raw to dps
          scale *= (float)M_PI / 180.f;                                  // Convert to radian.
          raw_data[0] = (long)short_data[0];
          raw_data[1] = (long)short_data[1];
          raw_data[2] = (long)short_data[2];
          inv_convert_dmp3_to_body(raw_data, scale, gyro_float);
        }  // header & GYRO_SET

        if (header & CPASS_CALIBR_SET) {
          float scale;
          dmp_get_calibrated_compass(long_data);
          scale = 1.52587890625e-005f; //COMPASS_CONVERSION
          inv_convert_dmp3_to_body(long_data, scale, compass_float);
        } // header & CPASS_CALIBR_SET

        if (header & CPASS_SET) {
          // Raw compass [DMP]
          dmp_get_raw_compass(long_data);
          compass_float[0] = long_data[0] * 1.52587890625e-005f;
          compass_float[1] = long_data[1] * 1.52587890625e-005f;
          compass_float[2] = long_data[2] * 1.52587890625e-005f;
        } // header & CPASS_SET

      }  // total_sample_cnt

      if (!data_left_in_fifo) break;
    } while (data_left_in_fifo);
    return FLN_OK;
  }
  else {    
    return FLN_ERR;
  }
}

int imu_init(unsigned short data_output_period_ms) {
  int result = 0;
  inv_set_chip_to_body_axis_quaternion(ACCEL_GYRO_ORIENTATION, 0.0);
  result |= inv_initialize_lower_driver(SERIAL_INTERFACE_I2C, 0);
  result |= inv_set_slave_compass_id(COMPASS_SLAVE_ID);
  if (result) {
    LOG_DEBUG("Could not initialize.\r\n");
    return FLN_ERR;
  }
  else {
    LOG_DEBUG("Initialized.\r\n");
  }
  result |= inv_set_gyro_fullscale(MPU_FS_500dps);
  result |= inv_set_accel_fullscale(MPU_FS_8G);
  result |= inv_enable_sensor(ANDROID_SENSOR_GYROSCOPE, 1);
  result |= inv_enable_sensor(ANDROID_SENSOR_ACCELEROMETER, 1);
  result |= inv_enable_sensor(ANDROID_SENSOR_GEOMAGNETIC_FIELD, 1);
  result |= inv_set_odr(ANDROID_SENSOR_GYROSCOPE, data_output_period_ms);
  result |= inv_set_odr(ANDROID_SENSOR_ACCELEROMETER, data_output_period_ms);
  result |= inv_set_odr(ANDROID_SENSOR_GEOMAGNETIC_FIELD, data_output_period_ms);
  result |= inv_reset_dmp_odr_counters();
  result |= dmp_reset_fifo();

  return result;
}
