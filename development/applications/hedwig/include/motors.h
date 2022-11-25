#ifndef MOTORS_H
#define MOTORS_H

#include "falcon_common.h"

#define MOTOR_1    0
#define MOTOR_2    1
#define MOTOR_3    2
#define MOTOR_4    3

void motors_init(void);
void motors_set_motor_us(uint8_t motor, uint16_t speed);
void motors_off(void);
void motors_emergency_stop(void);

#endif  // MOTORS_H
