#ifndef MOTORS_H
#define MOTORS_H

#include "falcon_common.h"

#define MOTOR1 1
#define MOTOR2 2
#define MOTOR3 3
#define MOTOR4 4

void motors_init(void);
void motors_set_motor_us(uint8_t motor, uint16_t speed);

#endif  // MOTORS_H
