#include "motors.h"
#include "bsp.h"

void motors_init(void)
{
  bsp_motors_init();

  DEBUG_LOG("Motors Init\r\n");

  motors_set_motor_us(1, 0);
  motors_set_motor_us(2, 0);
  motors_set_motor_us(3, 0);
  motors_set_motor_us(4, 0);

  HAL_Delay(1000);

  DEBUG_LOG("Motors Ready\r\n");
}

void motors_set_motor_us(uint8_t motor, uint16_t speed)
{
  ASSERT(speed <= 1000);
  bsp_motors_pwm_set_us(motor, speed+1000);
}

