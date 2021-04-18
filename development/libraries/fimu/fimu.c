/******************************************************************************
 * @file     fimu.c
 * @brief    Falcon IMU
 * @version  1.0
 * @date     2020-11-02
 * @author   Devin Bell
 ******************************************************************************/

#include "fimu.h"

signed char ACCEL_GYRO_ORIENTATION[9] = {0, 1, 0, 1, 0, 0, 0, 0, -1};
signed char COMPASS_ORIENTATION[9] = {0, -1, 0, 1, 0, 0, 0, 0, 1};

const unsigned char ACCEL_GYRO_CHIP_ADDR = 0x68;
const unsigned char COMPASS_SLAVE_ID = HW_AK09916;
const unsigned char COMPASS_CHIP_ADDR = 0x0C;
const unsigned char PRESSURE_CHIP_ADDR = 0x00;
long SOFT_IRON_MATRIX[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int a_average[3];
int g_average[3];
static int a_offset_reg_save[3];  // original accel offset register values
static int self_test_done = 0;

static int reset_offset_regs(void)
{
  int ret;
  uint8_t d[2];

  /* reset accel offset registers */
  if (self_test_done == 0) {
    /* save accel offset register values */
    LOG_DEBUG("Save the original accel offset register values\r\n");
    ret = inv_read_mems_reg(REG_XA_OFFS_H, 2, d);
    a_offset_reg_save[0] = (int)((d[0] << 8) | d[1]);  // ax
    ret |= inv_read_mems_reg(REG_YA_OFFS_H, 2, d);
    a_offset_reg_save[1] = (int)((d[0] << 8) | d[1]);  // ay
    ret |= inv_read_mems_reg(REG_ZA_OFFS_H, 2, d);
    a_offset_reg_save[2] = (int)((d[0] << 8) | d[1]);  // az
    if (ret) {
      LOG_DEBUG("Failed to read accel offset registers\r\n");
    }
  }
  else {
    /* restore accel offset registers to the original */
    LOG_DEBUG("Restore the original accel offset register values\r\n");
    d[0] = (a_offset_reg_save[0] >> 8) & 0xff;
    d[1] = a_offset_reg_save[0] & 0xff;
    ret = inv_write_single_mems_reg(REG_XA_OFFS_H, d[0]);
    ret |= inv_write_single_mems_reg(REG_XA_OFFS_H + 1, d[1]);
    d[0] = (a_offset_reg_save[1] >> 8) & 0xff;
    d[1] = a_offset_reg_save[1] & 0xff;
    ret |= inv_write_single_mems_reg(REG_YA_OFFS_H, d[0]);
    ret |= inv_write_single_mems_reg(REG_YA_OFFS_H + 1, d[1]);
    d[0] = (a_offset_reg_save[2] >> 8) & 0xff;
    d[1] = a_offset_reg_save[2] & 0xff;
    ret |= inv_write_single_mems_reg(REG_ZA_OFFS_H, d[0]);
    ret |= inv_write_single_mems_reg(REG_ZA_OFFS_H + 1, d[1]);
    if (ret) {
      LOG_DEBUG("Failed to reset accel offset registers\r\n");
    }
  }

  /* reset gyro offset registers */
  LOG_DEBUG("Reset gyro offset register values\r\n");
  d[0] = d[1] = 0;
  ret = inv_write_single_mems_reg(REG_XG_OFFS_USR_H, d[0]);
  ret |= inv_write_single_mems_reg(REG_XG_OFFS_USR_H + 1, d[1]);
  ret |= inv_write_single_mems_reg(REG_YG_OFFS_USR_H, d[0]);
  ret |= inv_write_single_mems_reg(REG_YG_OFFS_USR_H + 1, d[1]);
  ret |= inv_write_single_mems_reg(REG_ZG_OFFS_USR_H, d[0]);
  ret |= inv_write_single_mems_reg(REG_ZG_OFFS_USR_H + 1, d[1]);
  if (ret) {
    LOG_DEBUG("Failed to reset gyro offset registers\r\n");
  }

  return ret;
}

/* Set offset registers.
   accel_off[3] : accel offset (LSB @ 2g)
   gyro_off[3]  : gyro  offset (LSB @ 250dps) */
static int set_offset_regs(int accel_off[3], int gyro_off[3])
{
  int i, ret;
  uint8_t d[3];
  int bias[3];

  /* Accel offset registers */

  /* accel bias from self-test is 2g
     convert to 16g and mask bit0 */
  for (i = 0; i < 3; i++) {
    bias[i] = a_offset_reg_save[i] - (accel_off[i] >> 3);
    bias[i] &= ~1;
  }
  d[0] = (bias[0] >> 8) & 0xff;
  d[1] = bias[0] & 0xff;
  ret = inv_write_single_mems_reg(REG_XA_OFFS_H, d[0]);
  ret |= inv_write_single_mems_reg(REG_XA_OFFS_H + 1, d[1]);
  d[0] = (bias[1] >> 8) & 0xff;
  d[1] = bias[1] & 0xff;
  ret |= inv_write_single_mems_reg(REG_YA_OFFS_H, d[0]);
  ret |= inv_write_single_mems_reg(REG_YA_OFFS_H + 1, d[1]);
  d[0] = (bias[2] >> 8) & 0xff;
  d[1] = bias[2] & 0xff;
  ret |= inv_write_single_mems_reg(REG_ZA_OFFS_H, d[0]);
  ret |= inv_write_single_mems_reg(REG_ZA_OFFS_H + 1, d[1]);
  if (ret) {
    LOG_DEBUG("Failed to write accel offset registers\r\n");
  }

  /* Gyro offset registers */

  /* gyro bias from self-test is 250dps
     convert to 1000dps */
  for (i = 0; i < 3; i++) bias[i] = -(gyro_off[i] >> 2);

  d[0] = (bias[0] >> 8) & 0xff;
  d[1] = bias[0] & 0xff;
  ret = inv_write_single_mems_reg(REG_XG_OFFS_USR_H, d[0]);
  ret |= inv_write_single_mems_reg(REG_XG_OFFS_USR_H + 1, d[1]);
  d[0] = (bias[1] >> 8) & 0xff;
  d[1] = bias[1] & 0xff;
  ret |= inv_write_single_mems_reg(REG_YG_OFFS_USR_H, d[0]);
  ret |= inv_write_single_mems_reg(REG_YG_OFFS_USR_H + 1, d[1]);
  d[0] = (bias[2] >> 8) & 0xff;
  d[1] = bias[2] & 0xff;
  ret |= inv_write_single_mems_reg(REG_ZG_OFFS_USR_H, d[0]);
  ret |= inv_write_single_mems_reg(REG_ZG_OFFS_USR_H + 1, d[1]);
  if (ret) {
    LOG_DEBUG("Failed to write gyro offset registers\r\n");
  }
  return ret;
}

int fimu_init(fimu_config_t config)
{
  int result = 0;
  ICM_connect_i2c(config.i2cHandle);
  inv_sleep(10);
  inv_set_chip_to_body_axis_quaternion(ACCEL_GYRO_ORIENTATION, 0.0);
  result |= inv_initialize_lower_driver(SERIAL_INTERFACE_I2C, 0);
  result |= inv_set_slave_compass_id(COMPASS_SLAVE_ID);
  if (result) {
    LOG_DEBUG("Could not initialize.\r\n");
    return FLN_ERR;
  }
  else {
    LOG_DEBUG("Initialized.\r\n");
    return FLN_OK;
  }

  return FLN_OK;
}

int fimu_start(fimu_config_t config)
{
  int result = 0;

  result |= inv_set_gyro_divider(1U);   // Initial sampling rate 1125Hz/10+1 = 102Hz.
  result |= inv_set_accel_divider(1U);  // Initial sampling rate 1125Hz/10+1 = 102Hz.
  result |= inv_set_gyro_fullscale(config.gyro_fsr);
  result |= inv_set_accel_fullscale(config.accel_fsr);
  result |= inv_enable_sensor(ANDROID_SENSOR_GYROSCOPE, 1);
  result |= inv_enable_sensor(ANDROID_SENSOR_LINEAR_ACCELERATION, 1);
  result |= inv_enable_sensor(ANDROID_SENSOR_ROTATION_VECTOR, 1);
  unsigned short data_output_delay_ms = (unsigned short)(1000 / config.output_data_rate);
  result |= inv_set_odr(ANDROID_SENSOR_GYROSCOPE, data_output_delay_ms);
  result |= inv_set_odr(ANDROID_SENSOR_LINEAR_ACCELERATION, data_output_delay_ms);
  result |= inv_set_odr(ANDROID_SENSOR_ROTATION_VECTOR, data_output_delay_ms);

  result |= inv_reset_dmp_odr_counters();
  result |= dmp_reset_fifo();

  return result;
}

void fimu_fifo_handler(float *gyro_float, float *linAccFloat, float *quat_float)
{
  short int_read_back = 0;
  unsigned short header = 0, header2 = 0;
  int data_left_in_fifo = 0;
  short short_data[3] = {0};
  signed long long_data[3] = {0};
  signed long long_quat[3] = {0};
  unsigned short sample_cnt_array[GENERAL_SENSORS_MAX] = {0};

  float accel_float[3], rv_float[4];

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
          inv_convert_dmp3_to_body(raw_data, scale, gyro_raw_float);

          // We have gyro bias data in raw units, scaled by 2^5
          dmp_get_gyro_bias(short_data);
          scale = (1 << inv_get_gyro_fullscale()) * 250.f / (1L << 20);  // From raw to dps
          scale *= (float)M_PI / 180.f;                                  // Convert to radian.
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
        }  // header & GYRO_SET

        if (header & QUAT6_SET) {
          dmp_get_6quaternion(long_quat);

          long gravityQ16[3], temp_gravityQ16[3];
          long linAccQ16[3];
          long accelQ16[3];

          /*Calculate Gravity*/
          inv_convert_rotation_vector_1(long_quat, temp_gravityQ16);
          inv_mems_augmented_sensors_get_gravity(gravityQ16, temp_gravityQ16);

          /*Calculate Linear Acceleration*/
          accelQ16[0] = (int32_t)((float)(accel_float[0]) * (1ULL << 16) + ((accel_float[0] >= 0) - 0.5f));
          accelQ16[1] = (int32_t)((float)(accel_float[1]) * (1ULL << 16) + ((accel_float[1] >= 0) - 0.5f));
          accelQ16[2] = (int32_t)((float)(accel_float[2]) * (1ULL << 16) + ((accel_float[2] >= 0) - 0.5f));
          inv_mems_augmented_sensors_get_linearacceleration(linAccQ16, gravityQ16, accelQ16);
          linAccFloat[0] = inv_q16_to_float(linAccQ16[0]);
          linAccFloat[1] = inv_q16_to_float(linAccQ16[1]);
          linAccFloat[2] = inv_q16_to_float(linAccQ16[2]);
        }  // header & QUAT6_SET

        if (header & QUAT9_SET) {
          dmp_get_9quaternion(long_quat);

          inv_convert_rotation_vector(long_quat, rv_float);
          quat_float[0] = rv_float[0];
          quat_float[1] = -rv_float[3];
          quat_float[2] = rv_float[2];
          quat_float[3] = -rv_float[1];

        }  // header & QUAT9_SET

      }  // total_sample_cnt

      if (!data_left_in_fifo) break;
    } while (data_left_in_fifo);
  }
  else
    return;
}

int fimu_calibrate_DMP(void)
{
  LOG_DEBUG("Selftest started.\r\n");

  int self_test_result = 0;
  int dmp_bias[9] = {0};
  self_test_result = inv_mems_run_selftest();

  LOG_DEBUG("Selftest...Done...Ret=%d\r\n", self_test_result);
  LOG_DEBUG("Result: Compass=%s, Accel=%s, Gyro=%s\r\n", (self_test_result & 0x04) ? "Pass" : "Fail",
            (self_test_result & 0x02) ? "Pass" : "Fail", (self_test_result & 0x01) ? "Pass" : "Fail");
  LOG_DEBUG("Accel Average (LSB@FSR 2g)\r\n");
  LOG_DEBUG("\tX:%d Y:%d Z:%d\r\n", a_average[0], a_average[1], a_average[2]);
  LOG_DEBUG("Gyro Average (LSB@FSR 250dps)\r\n");
  LOG_DEBUG("\tX:%d Y:%d Z:%d\r\n", g_average[0], g_average[1], g_average[2]);

  if ((self_test_result & 0x03) != 0x03) {
    return FLN_ERR;
  }

  dmp_bias[0] = a_average[0] * (1 << 11);  // Change from LSB to format expected by DMP
  dmp_bias[1] = a_average[1] * (1 << 11);
  dmp_bias[2] = (a_average[2] - 16384) * (1 << 11);  // remove the gravity and scale (FSR=2 in selftest)
  int scale = 2000 / 250;                            // self-test uses 250dps FSR, main() set the FSR to 2000dps
  dmp_bias[3] = g_average[0] * (1 << 15) / scale;
  dmp_bias[4] = g_average[1] * (1 << 15) / scale;
  dmp_bias[5] = g_average[2] * (1 << 15) / scale;

  dmp_set_bias(dmp_bias);

  return FLN_OK;
}

int fimu_calibrate_offset(void)
{
  LOG_DEBUG("Selftest started.\r\n");

  int self_test_result = 0;
  int accel_offset[3], gyro_offset[3];
  int ret, i;

  ret = reset_offset_regs();
  if (ret) {
    LOG_DEBUG("Failed to reset offset registers\r\n");
  }
  self_test_result = inv_mems_run_selftest();

  LOG_DEBUG("Selftest...Done...Ret=%d\r\n", self_test_result);
  LOG_DEBUG("Result: Compass=%s, Accel=%s, Gyro=%s\r\n", (self_test_result & 0x04) ? "Pass" : "Fail",
            (self_test_result & 0x02) ? "Pass" : "Fail", (self_test_result & 0x01) ? "Pass" : "Fail");
  LOG_DEBUG("Accel Average (LSB@FSR 2g)\r\n");
  LOG_DEBUG("\tX:%d Y:%d Z:%d\r\n", a_average[0], a_average[1], a_average[2]);
  LOG_DEBUG("Gyro Average (LSB@FSR 250dps)\r\n");
  LOG_DEBUG("\tX:%d Y:%d Z:%d\r\n", g_average[0], g_average[1], g_average[2]);

  if ((self_test_result & 0x03) != 0x03) {
    return FLN_ERR;
  }

  // for ICM20648/20948/20608D/20609/20689
  for (i = 0; i < 3; i++) {
    if (i == 2) {
      // assume the device is put horizontal and z axis shows 1g during self-test.
      // self-test uses 2g FSR and 1g = 16384LSB
      accel_offset[i] = a_average[i] - 16384;
    }
    else {
      accel_offset[i] = a_average[i];
    }
    // gyro FSR is 250dps for self-test
    gyro_offset[i] = g_average[i];
  }
  /* Update offset registers */
  ret = set_offset_regs(accel_offset, gyro_offset);
  if (ret) {
    LOG_DEBUG("Failed to update offset registers\r\n");
    return FLN_ERR;
  }
  else {
    LOG_DEBUG("\r\nSetting the offset registers with one-axis factory calibration values...done\r\n");
    self_test_done = 1;
    return FLN_OK;
  }
}

void fimu_calibrate(float *gyro_bias, float *accel_bias, float *quat_bias)
{
  LOG_DEBUG("Starting IMU Cal\r\n");

  int samples = 50;

  float test_gyro[3];
  float test_accel[3];
  float test_quat[4];

  float gyro_samples[3] = {0.0F, 0.0F, 0.0F};
  float accel_samples[3] = {0.0F, 0.0F, 0.0F};
  float quat_samples[4] = {0.0F, 0.0F, 0.0F, 0.0F};

  for (int i = 0; i < samples; i++) {
    fimu_fifo_handler(test_gyro, test_accel, test_quat);
    LOG_DEBUG("Reading %d: gyro: %7.4f, %7.4f, %7.4f accel: %7.4f, %7.4f, %7.4f quat: %7.4f, %7.4f, %7.4f, %7.4f\r\n",
              i, test_gyro[0], test_gyro[1], test_gyro[2], test_accel[0], test_accel[1], test_accel[2], test_quat[0],
              test_quat[1], test_quat[2], test_quat[3]);

    gyro_samples[0] += test_gyro[0];
    gyro_samples[1] += test_gyro[1];
    gyro_samples[2] += test_gyro[2];
    accel_samples[0] += test_accel[0];
    accel_samples[1] += test_accel[1];
    accel_samples[2] += test_accel[2];
    quat_samples[0] += test_quat[0];
    quat_samples[1] += test_quat[1];
    quat_samples[2] += test_quat[2];
    quat_samples[3] += test_quat[3];

    rtos_delay_ms(10);
  }

  gyro_bias[0] = gyro_samples[0] / samples;
  gyro_bias[1] = gyro_samples[1] / samples;
  gyro_bias[2] = gyro_samples[2] / samples;

  accel_bias[0] = accel_samples[0] / samples;
  accel_bias[1] = accel_samples[1] / samples;
  accel_bias[2] = accel_samples[2] / samples;

  quat_bias[0] = quat_samples[0] / samples;
  quat_bias[1] = quat_samples[1] / samples;
  quat_bias[2] = quat_samples[2] / samples;
  quat_bias[3] = quat_samples[3] / samples;

  LOG_DEBUG("Bias: gyro: %7.4f, %7.4f, %7.4f accel: %7.4f, %7.4f, %7.4f quat: %7.4f, %7.4f, %7.4f, %7.4f\r\n",
            gyro_bias[0], gyro_bias[1], gyro_bias[2], accel_bias[0], accel_bias[1], accel_bias[2], quat_bias[0],
            quat_bias[1], quat_bias[2], quat_bias[3]);
}
