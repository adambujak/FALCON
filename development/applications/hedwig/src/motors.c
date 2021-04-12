#include "motors.h"
#include "bsp.h"

#include <stdbool.h>

static bool emergency_stop = false;

void motors_init(void)
{
  bsp_motors_init();

  LOG_DEBUG("Motors Init\r\n");

  motors_set_motor_us(MOTOR_1, 0);
  motors_set_motor_us(MOTOR_2, 0);
  motors_set_motor_us(MOTOR_3, 0);
  motors_set_motor_us(MOTOR_4, 0);

  delay_ms(1000);

  LOG_DEBUG("Motors Ready\r\n");
}

void motors_set_motor_us(uint8_t motor, uint16_t speed)
{
  if (!emergency_stop) {
    ASSERT(speed <= 1000);
    bsp_motors_pwm_set_us(motor, speed+1000);
  }
}

void motors_off(void)
{
  motors_set_motor_us(MOTOR_1, 0);
  motors_set_motor_us(MOTOR_2, 0);
  motors_set_motor_us(MOTOR_3, 0);
  motors_set_motor_us(MOTOR_4, 0);
}

void motors_emergency_stop(void)
{
  motors_off();
  emergency_stop = true;
}