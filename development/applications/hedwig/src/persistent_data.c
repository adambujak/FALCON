#include "persistent_data.h"

#include "falcon_packet.h"
#include "flash.h"

#include <string.h>

#define SECTOR               7
#define SECTOR_ADDRESS_START 0x08060000

typedef struct {
  ft_fcs_att_control_params_t att_control_params;
  ft_fcs_yaw_control_params_t yaw_control_params;
  ft_fcs_alt_control_params_t alt_control_params;
  float accel_bias[3];
  float gyro_bias[3];
} persistent_data_t;

static persistent_data_t *data;
static persistent_data_t data_buffer;


void persistent_data_init(void)
{
  data = (persistent_data_t *) SECTOR_ADDRESS_START;
  memcpy(&data_buffer, data, sizeof(data_buffer));
}

// call this after using setters to actually write the data to flash
void persistent_data_write(void)
{
  flash_write_unlock();
  flash_start_write(SECTOR);
  memcpy(data, &data_buffer, sizeof(data_buffer));
  flash_end_write(SECTOR);
  flash_write_lock();
}

void persistent_data_clear(void)
{
  flash_write_unlock();
  flash_erase_sector(SECTOR);
  flash_write_lock();
}

void persistent_data_controller_params_set(
       ft_fcs_att_control_params_t *att_control_params,
       ft_fcs_yaw_control_params_t *yaw_control_params,
       ft_fcs_alt_control_params_t *alt_control_params)
{
  data_buffer.att_control_params = *att_control_params;
  data_buffer.yaw_control_params = *yaw_control_params;
  data_buffer.alt_control_params = *alt_control_params;
}

void persistent_data_imu_bias_set(float accel_bias[3], float gyro_bias[3])
{
  memcpy(data_buffer.accel_bias, accel_bias, sizeof(accel_bias));
  memcpy(data_buffer.gyro_bias, gyro_bias, sizeof(gyro_bias));
}

void persistent_data_controller_params_get(
       ft_fcs_att_control_params_t *att_control_params,
       ft_fcs_yaw_control_params_t *yaw_control_params,
       ft_fcs_alt_control_params_t *alt_control_params)
{
  *att_control_params = data_buffer.att_control_params;
  *yaw_control_params = data_buffer.yaw_control_params;
  *alt_control_params = data_buffer.alt_control_params;
}

void persistent_data_imu_bias_get(float accel_bias[3], float gyro_bias[3])
{
  memcpy(accel_bias, data_buffer.accel_bias, sizeof(accel_bias));
  memcpy(gyro_bias, data_buffer.gyro_bias, sizeof(gyro_bias));
}

