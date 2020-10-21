#include "motors.h"
#include "bsp.h"

void motors_init(void)
{
  bsp_motors_init();
  motors_set_motor_us(1, 250);
  motors_set_motor_us(2, 500);
  motors_set_motor_us(3, 750);
  motors_set_motor_us(4, 1000);
}

void motors_set_motor_us(uint8_t motor, uint16_t speed)
{
  ASSERT(speed <= 1000);
  bsp_motors_pwm_set_us(motor, speed+1000);
}

